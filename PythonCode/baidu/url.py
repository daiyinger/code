import urllib2

str =  'http://www.tuling123.com/openapi/api?key='
str += '33b01a7ad9646872ee8292548486f47'
str += (raw_input('please input pass\n'))
str += '&info='
while True:
    strout = str + (raw_input('please input you want\n'))
    strout += '&userid=1234'
    print urllib2.urlopen(strout).read()
