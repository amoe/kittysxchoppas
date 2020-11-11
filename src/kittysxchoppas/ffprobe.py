import json
import subprocess
import locale

def ffprobe(format_option, path):
    raw_output = subprocess.check_output(
        [
            "ffprobe", "-v", "quiet", "-print_format",
            "json", "-skip_frame", "nokey", format_option, path
        ]
    )
    return json.loads(raw_output.decode(locale.getpreferredencoding()))
