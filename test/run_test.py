import subprocess
import pytest

def run_script(commands):
    executable = "../build/db"
    input_str = "\n".join(commands) + "\n"
    process = subprocess.Popen(
        [executable],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    stdout, stderr = process.communicate(input=input_str)

    if process.returncode != 0:
        pytest.fail(f"Script exited with error code {process.returncode}:\n{stderr}")
    return stdout.splitlines()

def test_inserts_and_retrieves_a_row():
    """
    Tests that we can insert a row and retrieve it.
    """
    commands = [
        "insert 0 user1 person1@example.com",
        "select",
        ".exit",
    ]

    actual_output = run_script(commands)
    expected_output = [
        "db > db > (0, user1, person1@example.com)",
        "db > ",
    ]

    assert set(actual_output) == set(expected_output)


def test_insert_with_long_strings_fails():
    """
    Tests that inserting strings that are too long for the buffer
    results in a proper error message and does not insert the row.
    """
    long_username = "a" * 33
    
    commands = [
        f"insert 1 {long_username} user@example.com",
        "select",
        ".exit",
    ]

    actual_output = run_script(commands)

    expected_output = [
        "db > Error, entered string is too long.",
        "db > db > ",
    ]

    assert set(actual_output) == set(expected_output)

def test_insert_with_negative_id_fails():
    """
    Tests that attempting to insert a row with a negative ID
    results in a proper error message.
    """
    commands = [
        "insert -1 cstack user@example.com",
        "select",
        ".exit",
    ]

    actual_output = run_script(commands)

    expected_output = [
        "db > Error, unable to parse statement.",
        "db > db > ",
    ]

    assert set(actual_output) == set(expected_output)

