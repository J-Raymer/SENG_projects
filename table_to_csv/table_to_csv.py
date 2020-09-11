import sys
import os
import argparse
import re
import csv

# Function that checks the extension of the file. if it is the right extension we call the opener function (eg:  argparse.FileType('r'))
# if its not the right extension we throw an error
def ext_check(expected_extension, openner):
    def extension(filename):
        base, file_extension = os.path.splitext(filename)
        if file_extension != expected_extension:
            sys.stderr.write(".html extension expected")
            raise ValueError()
        return openner(filename)
    return extension

def stripWhitespace(input_str):
    wordList = input_str.split() # split string into a list of words
    return ' '.join(wordList) # join our word list by 1 space ['Col', '1']

def pad_row(l, width):
    l.extend([''] * (width - len(l)))
    return l

def main():
    # define parser
    parser = argparse.ArgumentParser(description='table to csv')

    # arguments
    parser.add_argument('input_file', type=ext_check('.html', argparse.FileType('r')), help='An html file for input')
    parser.add_argument('output_file', type=ext_check('.txt', argparse.FileType('w')), help='A txt file for output')

    # get args
    args = parser.parse_args()

    with open(args.input_file.name) as file:
        with open(args.output_file.name, 'w') as csvFile:
            file_str = file.read().replace('\n', ' ')

            # get a list of strings in between all <table> tags
            table_data = re.findall(r'<table[^>]*>\s*((?:.|\n)*?)</table[ +]*>', file_str, re.DOTALL |  re.IGNORECASE)

            for i in range(len(table_data)):
                longest_row = 0
                table_output = []

                # get a list of strings in between all <tr> tags
                table_rows = re.findall(r'<tr[^>]*>\s*((?:.|\n)*?)</tr[ +]*>', table_data[i], re.DOTALL |  re.IGNORECASE)

                for row in table_rows:
                    # get a list of strings in between all <td> or <th> tags
                    cell_data = re.findall(r'<t[d|h][^>]*>\s*((?:.|\n)*?)</t[d|h][ +]*>', row, re.DOTALL |  re.IGNORECASE)
                    
                    if cell_data:
                        clean_cell_data = []
                        for cell in cell_data:
                            clean_cell_data.append( stripWhitespace(cell) )
                        if len(clean_cell_data) > longest_row:
                            longest_row = len(clean_cell_data)
                        table_output.append(clean_cell_data)
                
                # Write table_output to file
                writer = csv.writer(csvFile)
                writer.writerow(['TABLE ' + str(i+1) + ':'])
                
                for row in table_output:
                    writer.writerow(pad_row(row, longest_row))
                writer.writerow([])
        

    # fixes issue with extra empty line at bottom of file         
    with open(args.output_file.name, 'r') as csvFile:  
        lines = csvFile.readlines()
        lines.pop() 

    # Writes corrected data to file
    with open(args.output_file.name, 'w') as csvFile:  
        csvFile.writelines(lines) 

    # Prints corrected output to stdout
    with open(args.output_file.name, 'r') as csvFile:  
        file_contents = csvFile.read()
        print(file_contents)
        
if __name__ == '__main__':
    main()