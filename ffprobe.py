import json
import subprocess
import locale

def ffprobe(format_option, path):
    return json.loads(
        subprocess.check_output(
            [
                "ffprobe", "-v", "quiet", "-print_format",
                "json", "-skip_frame", "nokey", format_option, path
            ]
        ).decode(locale.getpreferredencoding())
    )
