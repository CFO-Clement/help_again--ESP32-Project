# Instructions.md

## Introduction
This document provides step-by-step instructions on setting up and running the provided Flask application which interfaces with a local JSON database and handles notification sending.

## Requirements
- Python 3.x
- Flask

## Setup Instructions
1. **Install Required Python Libraries**: Ensure you have Flask installed. You can install it using `pip`:

   ```bash
   pip install Flask
   ```

2. **Clone or Download the Source Code**: If you have not done so, get the provided source code onto your system.

3. **Database Setup**: Ensure the `local_db.json` database file exists in the same directory as your application. This JSON file will be used to store and retrieve data for the application.
This file must be in DynamoDB nosql format.  Here's an exemple:
    ```json
    {
        "id": {
            "S": "1"
        },
        "long_message": {
            "S": "J'ai besoin d'aide pour monter mes course"
        },
        "name": {
            "S": "Josette"
        },
        "short_message": {
            "S": "J'ai besoin d'aide pour decendre mes poubelle"
        },
        "status_help": {
            "S": "accepted"
        }
    }
    ```

## Running the Application
1. **Navigate to the Application Directory**:

   ```bash
   cd backend
   ```

2. **Run the Flask Application**:

   ```bash
   flask run
   ```


   Once running, you should see an output indicating that the server is running. By default, it will run on `http://localhost:8080` and will be availible for all the private network.

## Using the Application
The application has several routes:

1. **Send Short Notification**:
   - Route: `/short?client_id=YOUR_CLIENT_ID`
   - Method: `GET`
   - Action: Sends a short notification.

2. **Send Long Notification**:
   - Route: `/long?client_id=YOUR_CLIENT_ID`
   - Method: `GET`
   - Action: Sends a long notification.

3. **Accept Notification**:
   - Route: `/accept?client_id=YOUR_CLIENT_ID`
   - Method: `GET`
   - Action: Accepts the request and redirects to a Figma prototype.

4. **Deny Notification**:
   - Route: `/deny?client_id=YOUR_CLIENT_ID`
   - Method: `GET`
   - Action: Denies the request and redirects to another Figma prototype.

5. **Check Status**:
   - Route: `/status?client_id=YOUR_CLIENT_ID`
   - Method: `GET`
   - Action: Checks the status of a request.

6. **Error Handler**:
   - This is a built-in route to handle 404 (Page Not Found) errors. It returns a JSON response indicating the error.

Replace `YOUR_CLIENT_ID` with the respective client's ID when accessing the routes.

## Conclusion
Once you've set up and started the Flask application, you can interact with it using a web browser or tools like `curl` or Postman. Make sure to properly shut down the Flask server when you're done by pressing `CTRL+C` in the terminal where it's running.