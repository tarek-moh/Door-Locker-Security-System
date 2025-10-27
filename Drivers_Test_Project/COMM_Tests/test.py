import serial, time
import csv

# ====== Configuration ======
port = 'COM' + input("Enter COM port number from Device Manager (e.g., 3 for COM3): ")
baudrate = 9600

# ====== Command Codes ======
class CommandCode:
    CMD_READY = 0x10
    CMD_SEND_PASSWORD = 0x11
    CMD_PASSWORD_CORRECT = 0x12
    CMD_PASSWORD_WRONG = 0x13
    CMD_CHANGE_PASSWORD = 0x14
    CMD_DOOR_UNLOCK = 0x15
    CMD_DOOR_LOCK = 0x16
    CMD_ALARM = 0x17
    CMD_ACK = 0x18
    CMD_UNKNOWN = 0x19
    
# ====== Serial Setup ======
try:
    print(f"Opening serial port {port} at {baudrate} baud.")
    time.sleep(1)  # wait for serial port to be ready
    ser = serial.Serial(port, baudrate, timeout=1)
    print("Serial port opened successfully.")
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    time.sleep(3)
    exit(1)

# ====== Test Functions ======

results = {} # store test results here

# tests handshake by sending CMD_READY and expecting CMD_ACK
def hand_shake_test():
    ser.write(bytes([CommandCode.CMD_READY]))
    time.sleep(1) 
    response = ser.read()

    if response and response[0] == CommandCode.CMD_ACK:
        msg = f"[PASS] Handshake OK -> Received 0x{response[0]:02X}"
        print(msg)
        results['handshake'] = ('PASS', msg)
    else:
        msg = f"[FAIL] Handshake FAILED -> Received {response.hex() if response else 'nothing'}"
        print(msg)
        results['handshake'] = ('FAIL', msg)

# tests password entry by sending a password and expecting either CMD_PASSWORD_CORRECT or CMD_PASSWORD_WRONG
# both cases are tested by calls below.
def password_entry_test(label:str, password: bytes, expected_response: CommandCode):
    ser.write(bytes([CommandCode.CMD_SEND_PASSWORD]) + password)
    time.sleep(1)
    response = ser.read()
    if response and response[0] == expected_response:
        msg = f"[PASS] Password Entry '{password.decode().strip()}' received expected Control response 0x{expected_response:02X}"
        print(msg)
        results[label] = ('PASS', msg)
    else:
        msg = f"[FAIL] Password Entry '{password.decode().strip()}' -> received {response.hex() if response else 'nothing'}"
        print(msg)
        results[label] = ('FAIL', msg)

# tests door unlock by sending CMD_DOOR_UNLOCK and expecting CMD_ACK
# happy scenario only
def unlock_test():
    ser.write(bytes([CommandCode.CMD_DOOR_UNLOCK]))
    time.sleep(1)
    response = ser.read()
    if response and response[0] == CommandCode.CMD_ACK:
        msg = f"[PASS] Unlock test successful. received 0x{response[0]:02X}" 
        print(msg)
        results['unlock'] = ('PASS', msg)
    else:
        msg = f"[FAIL] Unlock test failed. received {response.hex() if response else 'nothing'}"
        print(msg)
        results['unlock'] = ('FAIL', msg)

# tests changing password by sending CMD_CHANGE_PASSWORD along with old and new passwords
# happy scenario only
def change_password_test(old_password: bytes, new_password: bytes):
    ser.write(bytes([CommandCode.CMD_CHANGE_PASSWORD]) + old_password + new_password)
    time.sleep(1)
    response = ser.read()
    if response and response[0] == CommandCode.CMD_ACK:
        msg = f"[PASS] Change password test successful. received 0x{response[0]:02X}"
        print(msg)
        results['change_password'] = ('PASS', msg)
    else:
        msg = f"[FAIL] Change password test failed. received {response.hex() if response else 'nothing'}"
        print(msg)
        results['change_password'] = ('FAIL', msg)

# ====== Run Tests ======

hand_shake_test()
password_entry_test('password_correct', b'1234\n', CommandCode.CMD_PASSWORD_CORRECT)
password_entry_test('password_wrong', b'0000\n', CommandCode.CMD_PASSWORD_WRONG)
unlock_test()
change_password_test(b'1234\n', b'5678\n')
ser.close() 

# ====== Save Results to CSV ======

with open('uart_test_results.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Test Name', 'Result'])
    # Here you would write the actual results of each test
    # For demonstration, we will just write placeholder results
    writer.writerow(['Handshake Test', results['handshake']])
    writer.writerow(['Password Entry Test (Correct)', results['password_correct']])
    writer.writerow(['Password Entry Test (Wrong)', results['password_wrong']])
    writer.writerow(['Unlock Test', results['unlock']])
    writer.writerow(['Change Password Test', results['change_password']])

while input("Press 'q' to quit: ") != 'q':
    pass
