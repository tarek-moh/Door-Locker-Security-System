import serial
import time
from datetime import datetime

# ====== Configuration ======
print("\n" + "=" * 60)
print("TIVA CONNECTION DIAGNOSTIC")
print("=" * 60)

# First, check all available ports
import serial.tools.list_ports

print("\n📡 Scanning for available COM ports...")
ports = serial.tools.list_ports.comports()

if not ports:
    print("✗ No COM ports found!")
    input("Press Enter to exit...")
    exit(1)

print(f"✓ Found {len(ports)} COM port(s):")
for port in ports:
    print(f"  • {port.device}: {port.description}")
    if "Stellaris" in port.description or "Tiva" in port.description:
        print(f"    → This looks like your Tiva board! ✓")

print("\n" + "=" * 60)
port = 'COM' + input("Enter COM port number from Device Manager (e.g., 3 for COM3): ")
baudrate = 9600


# ====== Command Codes (matching C enum order) ======
class CommandCode:
    CMD_READY = 0x10
    CMD_SEND_PASSWORD = 0x11
    CMD_PASSWORD_CORRECT = 0x12
    CMD_PASSWORD_WRONG = 0x13
    CMD_CHANGE_PASSWORD = 0x14
    CMD_DOOR_UNLOCK = 0x15
    CMD_DOOR_LOCK = 0x16
    CMD_SET_TIMEOUT = 0x17
    CMD_SUCCESS = 0x18
    CMD_FAIL = 0x19
    CMD_ALARM = 0x1A
    CMD_ACK = 0x1B
    CMD_UNKNOWN = 0x1C


# Command name mapping for display
COMMAND_NAMES = {
    0x10: "CMD_READY",
    0x11: "CMD_SEND_PASSWORD",
    0x12: "CMD_PASSWORD_CORRECT",
    0x13: "CMD_PASSWORD_WRONG",
    0x14: "CMD_CHANGE_PASSWORD",
    0x15: "CMD_DOOR_UNLOCK",
    0x16: "CMD_DOOR_LOCK",
    0x17: "CMD_SET_TIMEOUT",
    0x18: "CMD_SUCCESS",
    0x19: "CMD_FAIL",
    0x1A: "CMD_ALARM",
    0x1B: "CMD_ACK",
    0x1C: "CMD_UNKNOWN",
}

# ====== Serial Setup ======
print("\n" + "=" * 60)
print("OPENING SERIAL PORT")
print("=" * 60)

try:
    print(f"\n[1/4] Attempting to open {port} at {baudrate} baud...")
    ser = serial.Serial(port, baudrate, timeout=1)
    print(f"✓ Serial port {port} opened successfully")

    print("\n[2/4] Checking port properties...")
    print(f"  • Port: {ser.port}")
    print(f"  • Baudrate: {ser.baudrate}")
    print(f"  • Timeout: {ser.timeout}s")
    print(f"  • Is Open: {ser.is_open}")

    print("\n[3/4] Checking for any data in buffer...")
    waiting = ser.in_waiting
    if waiting > 0:
        print(f"  • Found {waiting} bytes in input buffer (probably startup messages)")
    else:
        print(f"  • Input buffer is empty")

    print("\n[4/4] Clearing buffers...")
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    print("✓ Buffers cleared - ready to communicate!\n")

except serial.SerialException as e:
    print(f"\n✗ ERROR: Could not open serial port!")
    print(f"  Error details: {e}")
    print(f"\n💡 Possible solutions:")
    print(f"  1. Close Code Composer Studio (CCS) completely")
    print(f"  2. Close any other serial terminal programs (PuTTY, TeraTerm, etc.)")
    print(f"  3. Unplug and replug the Tiva USB cable")
    print(f"  4. Try a different COM port from the list above")
    print(f"  5. Check Device Manager for any warning icons")
    input("\nPress Enter to exit...")
    exit(1)
except Exception as e:
    print(f"\n✗ UNEXPECTED ERROR: {e}")
    input("\nPress Enter to exit...")
    exit(1)


# ====== Helper Functions ======

def read_all_responses(timeout=2):
    """Read all available responses from Tiva"""
    responses = []
    start_time = time.time()

    while (time.time() - start_time) < timeout:
        if ser.in_waiting > 0:
            byte = ser.read(1)
            if byte:
                responses.append(byte[0])
                start_time = time.time()  # Reset timeout on data received
        else:
            time.sleep(0.05)

    return responses


def display_responses(responses):
    """Display responses in a readable format"""
    if not responses:
        print("  ← No response received")
        return

    print(f"  ← Received {len(responses)} byte(s):")
    for i, byte_val in enumerate(responses):
        # Check if it's a known command
        if byte_val in COMMAND_NAMES:
            print(f"     [{i}] 0x{byte_val:02X} = {COMMAND_NAMES[byte_val]}")
        elif 0x20 <= byte_val <= 0x7E:  # Printable ASCII
            print(f"     [{i}] 0x{byte_val:02X} = '{chr(byte_val)}'")
        else:
            print(f"     [{i}] 0x{byte_val:02X} (unknown)")


def establish_handshake():
    """Perform initial handshake with Tiva"""
    print("\n" + "=" * 60)
    print("ESTABLISHING CONNECTION WITH TIVA")
    print("=" * 60)

    print("\nSending CMD_READY (0x10) to Tiva...")
    ser.write(bytes([CommandCode.CMD_READY]))

    print("Waiting for CMD_READY response from Tiva...")
    responses = read_all_responses(timeout=5)

    if CommandCode.CMD_READY in responses:
        print("✓ Handshake successful! Tiva responded with CMD_READY")
        return True
    else:
        print("✗ Handshake failed. Tiva did not respond with CMD_READY")
        if responses:
            print("Received instead:")
            display_responses(responses)
        return False


def show_menu():
    """Display command menu"""
    print("\n" + "=" * 60)
    print("TIVA UART INTERACTIVE TEST TOOL")
    print("=" * 60)
    print("\n📋 Available Commands:")
    print("  1.  Send CMD_READY")
    print("  2.  Send CMD_SEND_PASSWORD (with password)")
    print("  3.  Send CMD_CHANGE_PASSWORD (with new password)")
    print("  4.  Send CMD_DOOR_UNLOCK")
    print("  5.  Send CMD_DOOR_LOCK")
    print("  6.  Send CMD_SET_TIMEOUT (with timeout value)")
    print("  7.  Send CMD_ALARM")
    print("  8.  Send CMD_ACK")
    print("  9.  Send Custom Command (enter hex code)")
    print("  10. Send Raw String Message")
    print("  11. Read Incoming Data (listen mode)")
    print("  12. Check Input Buffer (peek without reading)")
    print("  13. Clear Buffers")
    print("  14. Show this menu")
    print("  0.  Exit")
    print("=" * 60)


def send_cmd_ready():
    """Send CMD_READY"""
    print("\n→ Sending CMD_READY (0x10)")
    ser.write(bytes([CommandCode.CMD_READY]))
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def send_cmd_password():
    """Send CMD_SEND_PASSWORD with password"""
    password = input("  Enter password (e.g., 1234): ")
    password_bytes = (password + '\n').encode()

    print(f"\n→ Sending CMD_SEND_PASSWORD (0x11) + '{password}'")
    ser.write(bytes([CommandCode.CMD_SEND_PASSWORD]))
    ser.write(password_bytes)
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)

    # Ask if user wants to send ACK
    if responses and responses[0] in [CommandCode.CMD_PASSWORD_CORRECT, CommandCode.CMD_PASSWORD_WRONG]:
        ack = input("\n  Send ACK? (y/n): ")
        if ack.lower() == 'y':
            print("→ Sending CMD_ACK (0x18)")
            ser.write(bytes([CommandCode.CMD_ACK]))
            time.sleep(0.3)


def send_cmd_change_password():
    """Send CMD_CHANGE_PASSWORD with new passwords"""
    new_password = input("  Enter new password (e.g., 5678): ")

    new_pass_bytes = (new_password + '\n').encode()

    print(f"\n→ Sending CMD_CHANGE_PASSWORD (0x14) + new: '{new_password}'")
    ser.write(bytes([CommandCode.CMD_CHANGE_PASSWORD]))
    ser.write(new_pass_bytes)
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def send_cmd_door_unlock():
    """Send CMD_DOOR_UNLOCK"""
    print("\n→ Sending CMD_DOOR_UNLOCK (0x15)")
    ser.write(bytes([CommandCode.CMD_DOOR_UNLOCK]))
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def send_cmd_door_lock():
    """Send CMD_DOOR_LOCK"""
    print("\n→ Sending CMD_DOOR_LOCK (0x16)")
    ser.write(bytes([CommandCode.CMD_DOOR_LOCK]))
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def send_cmd_set_timeout():
    """Send CMD_SET_TIMEOUT with timeout value"""
    try:
        timeout_val = int(input("  Enter timeout value (5-30 seconds): "))
        print(f"\n→ Sending CMD_SET_TIMEOUT (0x17) + value: {timeout_val}")
        ser.write(bytes([CommandCode.CMD_SET_TIMEOUT, timeout_val]))
        time.sleep(0.5)
        responses = read_all_responses()
        display_responses(responses)
    except ValueError:
        print("  ✗ Invalid number")


def send_cmd_alarm():
    """Send CMD_ALARM"""
    print("\n→ Sending CMD_ALARM (0x1A)")
    ser.write(bytes([CommandCode.CMD_ALARM]))
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def send_cmd_ack():
    """Send CMD_ACK"""
    print("\n→ Sending CMD_ACK (0x1B)")
    ser.write(bytes([CommandCode.CMD_ACK]))
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def send_custom_command():
    """Send custom command by hex code"""
    try:
        hex_input = input("  Enter command code in hex (e.g., 0x15 or 15): ")
        if hex_input.startswith("0x") or hex_input.startswith("0X"):
            cmd_code = int(hex_input, 16)
        else:
            cmd_code = int(hex_input, 16)

        print(f"\n→ Sending custom command 0x{cmd_code:02X}")
        ser.write(bytes([cmd_code]))
        time.sleep(0.5)
        responses = read_all_responses()
        display_responses(responses)
    except ValueError:
        print("  ✗ Invalid hex value")


def send_raw_string():
    """Send raw string message"""
    message = input("  Enter string to send: ")
    add_newline = input("  Add newline? (y/n): ")

    if add_newline.lower() == 'y':
        message += '\n'

    print(f"\n→ Sending raw string: '{message}'")
    ser.write(message.encode())
    time.sleep(0.5)
    responses = read_all_responses()
    display_responses(responses)


def listen_mode():
    """Listen for incoming data"""
    print("\n👂 Listening mode (press Ctrl+C to stop)...")
    print("Waiting for data from Tiva...\n")

    try:
        byte_count = 0
        while True:
            if ser.in_waiting > 0:
                byte = ser.read(1)
                if byte:
                    byte_val = byte[0]
                    timestamp = datetime.now().strftime('%H:%M:%S.%f')[:-3]

                    # Show in multiple formats for debugging
                    if byte_val in COMMAND_NAMES:
                        print(f"[{timestamp}] 0x{byte_val:02X} = {COMMAND_NAMES[byte_val]}")
                    elif 0x20 <= byte_val <= 0x7E:
                        print(f"[{timestamp}] 0x{byte_val:02X} = '{chr(byte_val)}' (printable)")
                    else:
                        print(f"[{timestamp}] 0x{byte_val:02X} = {byte_val} decimal")

                    byte_count += 1
            time.sleep(0.01)
    except KeyboardInterrupt:
        print(f"\n✓ Stopped listening (received {byte_count} total bytes)")


def check_buffer():
    """Check what's in the input buffer without consuming it"""
    waiting = ser.in_waiting
    if waiting == 0:
        print("\n📭 Input buffer is empty")
    else:
        print(f"\n📬 Input buffer has {waiting} byte(s) waiting")
        print("  Reading them now...")
        responses = read_all_responses(timeout=0.5)
        display_responses(responses)


def clear_buffers():
    """Clear serial buffers"""
    before_input = ser.in_waiting
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    print(f"\n✓ Buffers cleared (removed {before_input} bytes from input buffer)")


# ====== Main Interactive Loop ======

def main():
    show_menu()

    while True:
        try:
            choice = input("\nEnter command number (14 for menu): ").strip()

            if choice == '0':
                print("\nClosing connection...")
                ser.close()
                print("✓ Goodbye!")
                break

            elif choice == '1':
                send_cmd_ready()

            elif choice == '2':
                send_cmd_password()

            elif choice == '3':
                send_cmd_change_password()

            elif choice == '4':
                send_cmd_door_unlock()

            elif choice == '5':
                send_cmd_door_lock()

            elif choice == '6':
                send_cmd_set_timeout()

            elif choice == '7':
                send_cmd_alarm()

            elif choice == '8':
                send_cmd_ack()

            elif choice == '9':
                send_custom_command()

            elif choice == '10':
                send_raw_string()

            elif choice == '11':
                listen_mode()

            elif choice == '12':
                check_buffer()

            elif choice == '13':
                clear_buffers()

            elif choice == '14':
                show_menu()

            else:
                print("  ✗ Invalid choice. Enter 14 to see menu.")

        except KeyboardInterrupt:
            print("\n\nExiting...")
            ser.close()
            break
        except Exception as e:
            print(f"\n✗ Error: {e}")


# ====== Entry Point ======
if __name__ == "__main__":
    main()