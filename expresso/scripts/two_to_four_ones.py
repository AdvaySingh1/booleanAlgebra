"""This module generates a 2-to-4 decoder with one-hot outputs."""

def generate():
    """_summary_
        Output format looks like this:
            101011  1
        Where there's 6 ones.If there's 2, 3, or 4 ones,
        the output should be one.
    """
    for i in range(0, 111111 + 1):
        s = str(i).zfill(6)
        if s.count('1') + s.count('0') == 6:
            print(f"{s} {int(s.count('1') in [2, 3, 4])}")
    pass




if __name__=="__main__":
    generate()