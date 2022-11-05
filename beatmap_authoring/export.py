import numpy as np
import csv
import sys
import os

def export_beatmap(name):

    ups = 0
    downs = 0

    curr_path = os.path.dirname(os.path.realpath(__file__)) + "/"
    fname = curr_path + name + ".csv"
    fname_out = curr_path + name + ".beatmap"

    # load bytes from file
    with open(fname_out, "wb") as b:
        # load csv file as string
        with open(fname, 'r') as f:
            reader = csv.reader(f)
            beats = list(reader)
            for beat in beats:
                # convert time to ms
                time_ms = float(beat[0]) * 1000
                time_ms = np.uint32(time_ms)
                print(time_ms)

                # convert key to uint8
                key = np.uint8(beat[1])
                print(key)
                if (key == 0):
                    ups += 1
                if (key == 1):
                    downs += 1

                # turn time and key into byte array
                time_bytes = np.uint32(time_ms).tobytes()
                key_bytes = np.uint8(key).tobytes()
                total_bytes = time_bytes + key_bytes

                # write bytes to file
                b.write(total_bytes)

    print("ups: " + str(ups))
    print("downs: " + str(downs))
        
if __name__ == '__main__':
    export_beatmap(sys.argv[1])