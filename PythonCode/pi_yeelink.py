#!/usr/bin/env python
# -*- coding: utf-8 -*-
import requests
import json
import time

def main():
    while True:

        # 设备URI
        apiurl = 'http://api.yeelink.net/v1.1/device/16993/sensor/381993/datapoints'
        # 用户密码, 指定上传编码为JSON格式
        apiheaders = {'U-ApiKey': '9bedf55c4da4d3555218bffed9801080', 'content-type': 'application/json'}
        # 字典类型数据，在post过程中被json.dumps转换为JSON格式字符串 {"value": 1}
	values = 1
	payload = {"value":values}
        #发送请求
        r = requests.post(apiurl, headers=apiheaders, data=json.dumps(payload))
	print('res',r)
	break

if __name__ == '__main__':
    main()
    
