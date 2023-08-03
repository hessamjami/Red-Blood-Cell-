from datetime import timedelta
from datetime import datetime

def get_simulation_time_stamp():
    """Get VCM launch time.

    Returns:
        str: String formatted with date and time.
    """
    now = datetime.now()
    dt_string = f"{now.year}_{now.month:02}_{now.day:02}_time_{now.hour:02}_{now.minute:02}"
    return dt_string

def get_string_for_time_unit(delta_time, time_unit):
    """Create a string to indicate the lapsed time.

    Args:
        delta_time (int): Lapsed time.
        time_unit (str): Unit of time in singular form.

    Returns:
        str: Formatted time with units.
    """
    delta_time_str = f"{delta_time}\t{time_unit}"
    if delta_time != 1:
        delta_time_str += "s"
    delta_time_str += "\n"
    return delta_time_str


def parse_time_delta(delta_time_in_seconds):
    """Convert total lapsed seconds into days, hours, minutes, and seconds.

    Args:
        delta_time_in_seconds (float): Lapsed time in seconds.

    Returns:
        dict: dictionary with integer values of lapsed days, hours, minutes, and seconds.
    """
    seconds_per_hour = 3600
    seconds_per_min = 60
    days = delta_time_in_seconds.days
    total_seconds = delta_time_in_seconds.seconds
    hours, remainder = divmod(total_seconds, seconds_per_hour)
    minutes, seconds = divmod(remainder, seconds_per_min)
    delta_t = {"day": days, "hour": hours, "minute": minutes, "second": seconds}
    return delta_t


def print_runtime(delta_clock, title):
    """Format and print string that reports the duration of simulation runtime.

    Args:
        delta_clock (float): Lapsed tim in seconds.
        title (str): Title of the lapsed time value.

    Returns:
        str: Formatted runtime duration.
    """
    delta = timedelta(seconds=delta_clock)
    delta_t = parse_time_delta(delta)
    output_str = f"{title}:\n"
    for time_unit, delta_time in delta_t.items():
        if delta_time != 0:
            temp_str = get_string_for_time_unit(delta_time, time_unit)
            output_str += f"{temp_str}"
    if output_str == f"{title}:\n":
        seconds_str = get_string_for_time_unit(delta_t["second"], "second")
        output_str += f"{seconds_str}"
    output_str += "\n"
    print(output_str)
    return output_str