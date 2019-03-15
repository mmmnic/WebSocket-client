Using the Nuvoton NUC131 and NodeMCU

// NUVOTON NUC131
The Nuvoton NUC131 is chosen to handle receive data when user push buttons then send data to NodeMCU through UART

// NODEMCU
The NodeMCU has to connect to wifi and then wait for the data.
If the NodeMCU received data from the NUC131 then it convert data into a JSON string.
after converted the JSON string will be sent to the server.
