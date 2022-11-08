import numpy as np
import csv
import sys
import os

# 0 = up
# 1 = down
# 2 = left
# 3 = right

# alternating patterns: (digits go pattern, number of notes, start key)
# same: 0(3, 4, 5, 6)(0, 1, 2, 3)
# opposite: 1(3, 4, 5, 6)(0, 1, 2, 3)
# seq +: 2(3, 4, 5, 6)(0, 1, 2, 3)
# seq -: 3(3, 4, 5, 6)(0, 1, 2, 3)
# special cases: 4(3)(0, 1): up/down then left, right
#                5(3)(0, 1): up/down then right, left

def seq_plus (n):
    return (n + 1) % 4
def seq_minus (n):
    return (n - 1) % 4
def opposite (n):
    if (n == 0):
        return 1
    if (n == 1):
        return 0
    if (n == 2):
        return 3
    if (n == 3):
        return 2

def expand_beatmap(beats):
    expanded_beats = []
    for beat in beats:
        # convert time to ms
        time_s = float(beat[0])

        # convert key to uint
        key = np.uint32(beat[1])

        if (key < 10):
            expanded_beats.append(beat)
        else:
            # get pattern
            pattern = key // 100
            # get key
            num_notes = (key // 10) % 10
            # get number of notes
            start_key = key % 10

            print("pattern: " + str(pattern) + " num_notes: " + str(num_notes) + " start_key: " + str(start_key))

            # get next key timestamp
            next_ind = beats.index(beat) + 1
            print(beats.index(beat))
            print(next_ind)
            if (next_ind > len(beats) - 1):
                # extrapolate
                next_ind = len(beats) - 2
                prev_time = float(beats[next_ind][0])
                dt = (time_s - prev_time) / num_notes
            else:
                next_time = float(beats[next_ind][0])
                print(next_time)
                dt = (next_time - time_s) / num_notes
                print(dt)
            
            # check special case
            if (pattern == 4):
                assert(num_notes == 3)
                # up/down then left, right
                expanded_beats.append([time_s, start_key])
                expanded_beats.append([time_s + dt, 2])
                expanded_beats.append([time_s + 2 * dt, 3])
                continue
            elif (pattern == 5):
                assert(num_notes == 3)
                # up/down then right, left
                expanded_beats.append([time_s, start_key])
                expanded_beats.append([time_s + dt, 3])
                expanded_beats.append([time_s + 2 * dt, 2])
                continue

            written_key = start_key
            for i in range(0, num_notes):
                
                # append with time and key converted to string
                expanded_beats.append([str(time_s + i * dt), str(written_key)])
               
                if (pattern == 1):
                    written_key = opposite(written_key)
                elif (pattern == 2):
                    written_key = seq_plus(written_key)
                elif (pattern == 3):
                    written_key = seq_minus(written_key)

            

    print(expanded_beats)
    print(len(expanded_beats))
    return expanded_beats

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

            # write length of beatmap as first line
            b.write(np.uint32(0).to_bytes())
            b.write(np.uint8(len(beats)).to_bytes())
            
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


def export_beatmap_expand (name):
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
            beats = expand_beatmap(list(reader))

            # write length of beatmap as first line
            b.write(np.uint32(0).tobytes())
            b.write(np.uint8(len(beats)).tobytes())

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
    # name = sys.argv[1]
    # curr_path = os.path.dirname(os.path.realpath(__file__)) + "/"
    # fname = curr_path + name + ".csv"
    # fname_out = curr_path + name + ".beatmap"

    # # load bytes from file
    # with open(fname_out, "wb") as b:
    #     # load csv file as string
    #     with open(fname, 'r') as f:
    #         reader = csv.reader(f)
    #         beats = list(reader)
    #         expand_beatmap(beats)

    export_beatmap_expand(sys.argv[1])