import numpy as np
import csv
from datetime import datetime

def export_beatmap(fname):
    # load bytes from file
    with open("export.beatmap", "wb") as b:
        # load csv file as string
        with open(fname, 'r') as f:
            reader = csv.reader(f)
            beats = list(reader)
            for beat in beats:
                # convert time to datetime
                time = datetime.strptime(beat[0], "%M:%S.%f")
                # convert time to milliseconds
                time_ms = time.minute * 60 * 1000 + time.second * 1000 + time.microsecond / 1000
                print(time_ms)

                # convert key to uint8
                key = np.uint8(beat[1])
                print(key)

                # turn time and key into byte array
                time_bytes = np.uint32(time_ms).tobytes()
                key_bytes = np.uint8(key).tobytes()
                total_bytes = time_bytes + key_bytes

                # write bytes to file
                b.write(total_bytes)
        
if __name__ == '__main__':
    export_beatmap('proto_beatmap.csv')