from flask import Flask, request, jsonify, redirect
import json
from typing import Dict, Union, Any

app = Flask(__name__)

DATABASE = "local_db.json"

ENDPOINT = "http://localhost:5000"

def load_db() -> Dict:
    """
    Loads the database from the file system.

    Returns:
        dict: The database contents.
    """
    try:
        with open(DATABASE, 'r') as file_handle:
            return json.load(file_handle)
    except FileNotFoundError:
        raise Exception("Local database not found.")

def write_db(data: Dict[str, Union[str, int]]) -> None:
    """Write the data to the database file."""
    try:
        with open(DATABASE, 'w') as file:
            json.dump(data, file)
    except OSError as err:
        raise DatabaseError("Cannot write to database") from err

def get_db_entry(id: str) -> Dict[str, Any]:
    """Return the entry for the given ID, or an empty dictionary if there is no entry."""
    data = load_db()
    if not isinstance(data, dict):
        raise RuntimeError(f"Expected data to be a dictionary, but found {type(data)}")
    return data.get(id, {})

def update_db_entry(id: str, updates: Dict) -> None:
    """Update the DB entry for a given ID with provided updates."""
    data = load_db()
    if id not in data:
        data[id] = {}
    for key, value in updates.items():
        data[id][key] = value
    write_db(data)

def notification_template(message:str , name:str, client_id:str) -> dict:
    payload:dict = {
        "topic": "workshop",
        "title": f"Vous avez recu une demande d'aide de la part de {name}",
        "message": message,
        "actions": [
            {
                "action": "view",
                "label": "Accepter",
                "url": f"{ENDPOINT}/accept?client_id={client_id}",
                "clear": True
            },
            {
                "action": "view",
                "label": "Refuser",
                "url": f"{ENDPOINT}/deny?client_id={client_id}",
                "clear": True
            }
        ],
        "icon": "https://projet-workshop.s3.eu-west-3.amazonaws.com/MicrosoftTeams-image__1_-removebg-preview.png"
    }
    return payload

def send_notif(payload: dict) -> bytes:
    """
    Sends a notification using the ntfy.sh service.

    Args:
        payload: The notification payload.

    Returns:
        The response from the ntfy.sh service.
    """
    try:
        req: request.Request = request.Request(
            url="https://ntfy.sh/",
            data=json.dumps(payload).encode('utf-8'),
            headers={
                'Content-Type': 'application/json',
                'User-Agent': 'Mozilla/5.0',
            },
            method='POST'
        )

        with request.urlopen(req) as response:
            return response.read()
    except Exception as e:
        print(f"An error occurred: {e}")
        return None

def send_short(id: str):
    """
    This function query the short message from the database and send it to the client using send_notif function

    Args:
        id: The client id

    Returns:
        200 HTPP code
    """
    update_db_entry(id, {"status_help": {"S": "pending"}})
    data = get_db_entry(id)
    name = data.get("name", {}).get("S", "")
    short_message = data.get("short_message", {}).get("S", "")
    payload = notification_template(message=short_message, name=name, client_id=id)
    send_notif(payload)
    return {
        "statusCode": 200,
        "body": json.dumps({
            "message": "Notification sent"
        })
    }

def send_long(id: str):
    """
    This function query the long message from the database and send it to the client using send_notif function

    Args:
        id: The client id

    Returns:
        200 HTPP code
    """
    update_db_entry(id, {"status_help": {"S": "pending"}})
    data = get_db_entry(id)
    name = data.get("name", {}).get("S", "")
    long_message = data.get("long_message", {}).get("S", "")
    payload = notification_template(message=long_message, name=name, client_id=id)
    send_notif(payload)
    return {
        "statusCode": 200,
        "body": json.dumps({
            "message": "Notification sent"
        })
    }

def accept(id: str):
    """This function is called when the helper accept the request then redirect him to the figma prototype

    Args:
        id (str): The client id

    Returns:
        302 HTTP code
    """
    update_db_entry(id, {"status_help": {"S": "accepted"}})
    return {
        "statusCode": 302,
        "headers": {
            "Location": "https://www.figma.com/proto/t69P2mjsn3xhactmIQu9dt/Untitled?type=design&node-id=80-197&t=emxTBzgDA3YPQ2wg-0&scaling=scale-down&page-id=0%3A1&starting-point-node-id=80%3A197&show-proto-sidebar=1"
        }
    }

def deny(id: str):
    """This function is called when the helper deny the request then redirect him to the figma prototype

    Args:
        id (str): The client id

    Returns:
        302 HTTP code
    """
    update_db_entry(id, {"status_help": {"S": "denied"}})
    return {
        "statusCode": 302,
        "headers": {
            "Location": "https://www.figma.com/proto/t69P2mjsn3xhactmIQu9dt/Untitled?type=design&node-id=80-284&t=emxTBzgDA3YPQ2wg-0&scaling=scale-down&page-id=0%3A1&starting-point-node-id=80%3A284&show-proto-sidebar=1"
        }
    }

def get_status(id: str):
    """This function is called when the ESP32 want to know the status of his request

    Args:
        id (str): the client id

    Returns:
        Union['pending'|'accepted'|'denied']: The state of the request
    """
    data = get_db_entry(id)
    status = data.get("status_help", {}).get("S", "")
    return {
        "statusCode": 200,
        "body": status
    }


@app.route('/short', methods=['GET'])
def short():
    id_client = request.args.get('client_id')
    return send_short(id_client)

@app.route('/long', methods=['GET'])
def long():
    id_client = request.args.get('client_id')
    return send_long(id_client)

@app.route('/accept', methods=['GET'])
def accept_route():
    id_client = request.args.get('client_id')
    return accept(id_client)

@app.route('/deny', methods=['GET'])
def deny_route():
    id_client = request.args.get('client_id')
    return deny(id_client)

@app.route('/status', methods=['GET'])
def status_route():
    id_client = request.args.get('client_id')
    return get_status(id_client)

@app.errorhandler(404)
def page_not_found(e):
    return jsonify({
        "statusCode": 404,
        "message": "Route not found"
    }), 404

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=8080)
