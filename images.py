import os
import PIL.Image as Image 
import test


'''
add the first n characters from the first file to a set and return the set 
'''
def get_set(size):
    result = set()
    f = open("1001-c.gnt", "rb")
    for i in range(size):
        data = test.process_char(f)
        result.add(data[0])
    return result


'''
Loop through all the files, writing only the data for characters in char_set
to new files.
'''
def write_file(char_set):
    for fnum in range(1001, 1301):
        print(fnum)
        fname = str(fnum) + "-c.gnt"
        fname2 = str(fnum) + "-bmps.gnt"  
        f = open(fname, "rb")
        f2 = open(fname2, "rb")
        output = open("small.gnt", "ab")
        entry_sizes = 0
        f_size = os.path.getsize(fname)
        while entry_sizes < f_size:
            data = test.process_char(f)
            bmp = f2.read(900)
            if (data[0] in char_set):
                output.write(data[5]) #gb code
                output.write(bmp)
            entry_sizes += data[4]
    
def main():
    #char_set = get_set(50)
    #write_file(char_set)
    f = open("1008-c.gnt", "rb")
    data = test.process_char(f)
    width = data[2]
    height = data[3]
    img = Image.new("1", (width, height))
    pixels = img.load()
    pos = 0
    for j in range(height):
        for i in range(width):
            pixels[i, j] = data[1][pos] 
            pos += 1
    img.save("original2.bmp")
    new_img = test.resize_bmp(img, width, height, 50, 50)
    new_img.save("50x502.bmp")
    
if __name__ == "__main__":
    main()
