# DocsEditCLI
TCP-based multi-client online file editing and sharing console-based application. Currently supports a maximum of five simultaneous clients. Uses the POSIX sockets library.

# Features

### Uploading Files
Each client can upload a file of its choice.
### Downloading Files
Each client can download the files.
### View Actvie client
Each client can view or set permission for file for read and write access.
### Client can request for collaboration or Provide access to other Clients
Each client can send request to other active client for collaboration by sending invites to clients and can also set different permission for files for different clients.
### Multiple Client can perform CRUD operation on files
Multiple client can perform CRUD operation on files provided they have access for operations


# How to Run
To start Server
Compile the program with
```bash
gcc server.c
```
To start Client
Complile the program with
```bash
gcc client.c
```

## Note:
The clients must be started in separate terminal
