#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
获取运行script之后的页面返回
1). sudo pip install selenium
2). 安装三大浏览器驱动driver
     1.chromedriver
　 http://chromedriver.storage.googleapis.com/index.html

     2.Firefox的驱动geckodriver 
     https://github.com/mozilla/geckodriver/releases/

     3.IE的驱动IEdriver
     http://www.nuget.org/packages/Selenium.WebDriver.IEDriver/
    注意：下载解压后，将chromedriver , geckodriver , Iedriver 放到某个目录，
    例如 ~/bin 。 然后再将~/bin到系统环境变量的Path下面。
 
 3). 
    from selenium import webdriver
    
    webdriver.Chrome()
    webdriver.Firefox()
    webdriver.Ie()

4).
'''
from selenium import webdriver


def get_url_dynamic2(url):
     # driver = webdriver.Firefox() 
     # driver = webdriver.Chrome()       #会打开一个浏览器窗口 
     # driver.maximize_window() # chrome窗口最大化
     options = webdriver.ChromeOptions()
     options.add_argument('headless')   #不会打开Chrome浏览器
     driver = webdriver.Chrome(chrome_options=options)

     driver.get(url) #请求页面
     html_text = driver.page_source
     driver.quit()
     #print html_text
     return html_text


url = 'http://lottery.sina.com.cn/ssqleitai/'
print get_url_dynamic2(url)
