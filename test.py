import os
import numpy
import PIL.Image as Image 
import math

def process_char(file):
    entry_size = int.from_bytes(file.read(4), byteorder = "little")

    #each gb code corresponds to a certain chinese character
    gb_code = file.read(2)
    correct_character = gb_code.decode("GB2312")
    
    width = file.read(2)
    height = file.read(2)
    width_int = int.from_bytes(width, byteorder='little')
    height_int = int.from_bytes(height, byteorder='little')

    
    bmp_size = width_int * height_int 
    image_bytes = file.read(bmp_size)
    img_list = []
    for b in image_bytes:
        if b == 255:
            b = 1
        else:
            b = 0
        img_list.append(b)
    
    return [correct_character, img_list, width_int, height_int, entry_size]

def get_training_data(file_num):
    biggest_sz = 0
    correct_char_list = []
    img_list = []
    char_size_list = []
    char_set = set()
    data_lst = []
    file_name = str(1000 + file_num) + "-c.gnt"
    with open(file_name, "rb") as f:
        f_size = os.path.getsize(file_name)
        entry_sizes = 0
        while entry_sizes < f_size:
            char_data = process_char(f)
            entry_sizes += char_data[4]
            correct_char_list.append(char_data[0])
            char_set.add(char_data[0])
            img_list.append(char_data[1])
            char_size_list.append((char_data[2], char_data[3]))
            if (biggest_sz < char_data[4] - 10):
                biggest_sz = char_data[4] - 10
            for img in img_list:
                while len(img) != biggest_sz:
                    img.append(0)
                
            img_arr = numpy.asarray(img_list)
            data_lst.append((img_arr , char_data[0]))            
            
    return (correct_char_list, img_arr, char_size_list, char_set, biggest_sz, data_lst)

'''
given a PIL image, and noninteger coordinates, return the pixel of that image
that is closest to the coordinates in Euclidian distance.
'''
def nearest_neighbor(bmp, x, y):
    #round down 
    x_floor = math.floor(x)
    y_floor = math.floor(y)
    
    #find amount rounded down
    dx = x-x_floor
    dy = y-y_floor
    
    #these 4 cases are the 4 possible closest pixels
    if (dx > 0.5 and dy > 0.5):
        x_nn = x_floor + 1
        y_nn = y_floor + 1
    elif(dx > 0.5 and dy < 0.5):
        x_nn = x_floor + 1
        y_nn = y_floor
    elif(dx < 0.5 and dy > 0.5):
        x_nn = x_floor
        y_nn = y_floor + 1
    else:
        x_nn = x_floor
        y_nn = y_floor
    
    if (x_nn >= bmp.size[0]):
        x_nn -= 1
    if (y_nn >= bmp.size[1]):
        y_nn -= 1
    
    pixels = bmp.load()
    return pixels[x_nn, y_nn]
        
'''
Given a PIL image, its current width and height, and the desired width and 
height, return a new resized image with the new width and height.
'''
def resize_bmp(bmp, width, height, new_width, new_height):
    #to convert locations on new image to locations on old image
    w_ratio = width / new_width
    h_ratio = height / new_height
    
    result = Image.new("1", (new_width, new_height))
    pixels = result.load()
    for j in range(result.size[1]): #height
        for i in range(result.size[0]): #width
            x = i * w_ratio #corresponding location on old bitmap
            y = j * h_ratio
            
            #assign current pixel
            pixels[i, j] = nearest_neighbor(bmp, x, y)
    return result
    

'''
Loop through all 300 files from CASIA database, resize the bmp image to 30 x 30
and write the new bmps to another file.
'''
def main():
    for k in range(300):
        fnum = 1001 + k
        fname = str(fnum) + "-c.gnt"
        fname2 = str(fnum) + "-bmps.gnt"
        f = open(fname, "rb")
        f2 = open(fname2, "wb")
        entry_sizes = 0
        f_size = os.path.getsize(fname)
        
        while entry_sizes < f_size:
            data = process_char(f)
            entry_sizes += data[4]
            width = data[2]
            height = data[3]
            img = Image.new("1", (width, height))
            img_pixels = img.load()
            pos = 0
            for j in range(height):
                for i in range(width):
                    img_pixels[i, j] = data[1][pos]
                    pos += 1
            new_img = resize_bmp(img, width, height, 30, 30)
            pixels = new_img.load()
            for j in range(new_img.size[1]):
                for i in range(new_img.size[0]):    
                    f2.write(pixels[i, j].to_bytes(1, byteorder = 'little'))

if __name__ == "__main__":
    main()
