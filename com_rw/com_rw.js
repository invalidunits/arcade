const { SerialPort } = require('serialport')

const COM_PORT = "COM4"|| process.env.COM_PORT

const Serial = new SerialPort({"path": COM_PORT, baudRate: 115200});
const stdin = process.openStdin();

stdin.addListener("data", (data) => {
    Serial.write(data);
})

Serial.addListener("data", (data) => {
    process.stdout.write(data);
})