import Image
import sys
import pytesser 
tools = pyocr.get_available_tools()[:]
if len(tools) == 0:
    print("No OCR tool found")
    sys.exit(1)
print("Using '%s'" % (tools[0].get_name()))
print(pytesser.image_file_to_string('test.jpg'))
