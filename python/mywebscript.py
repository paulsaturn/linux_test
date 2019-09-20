#!/usr/bin/env python
# -*- coding: utf-8 -*-

from selenium import webdriver
from selenium.webdriver.common.keys import Keys

b = webdriver.Firefox()
print('Visitinq github')
b.get('http://github.com')
print('Performing search')
# 父元素:
#  (#)表示通过id属性来定位元素
#  (.)表示通过class属性来定位元素
# 子元素（后代关系）
#  #choose_car option    含空格的，后面的元素不必是前面元素的直接子元素，只要在父元素的里面
#  .s_ipt_wr span        
#  #choose_car > option  含>键的，后面的元素必须是前面元素的直接子元素
#  .s_ipt_wr > span
#  ul > ol > li > em     可以是很多级的父子关系
searchselector = '.js-site-search-form input[type="text"]'
searchbox = b.find_element_by_css_selector(searchselector)
searchbox.send_keys('docker-in-practice')
print('Switching to user search')
# 查找具体的元素，必须在前面输入标准开头//，表示从当前节点寻找所有的后代元素
# //div/* div下面的所有的元素
# //div//p 先在整个文档里查找div，再在div里查找p节点(只要在内部，不限定是否紧跟) ；等价于 css_selector里的('div p')
# //div/p p是div的直接子节点； 等价于 css_selector里的('div > p')
# //*[@style] 查找所有包含style的所有元素，所有的属性要加@； 等价于 css_selector里的('*[style]')
# //p[@spec='len'] 必须要加引号；等价于 css_selector里的("p[spec='len']")
# //p[@id='kw'] xpath中对于id,class与其他元素一视同仁，没有其他的方法
# 在web element对象里面使用查找xpath查找时，必须使用.指明当前节点, 如.//p

# 1).错误
# usersxpath = '//nav//a[contains(text(), "Users")]'
# userslink = b.find_element_by_xpath(usersxpath)
# userslink.click()
# 2).正确
# searchbox.send_keys(Keys.ENTER)
# 3).正确
searchbox.click()
userslink = b.find_element_by_id('jump-to-results')
userslink.click()

print('Openning docker in practice user page')
dlinkselector = '.repo-list-item a'
dlink = b.find_elements_by_css_selector(dlinkselector)[0]
dlink.click()

# ss = b.find_element_by_class_name('tK1')      # 定位
# print(ss.get_attribute("innerHTML"))          # 用innerHTML 会返回元素的内部 HTML， 包含所有的HTML标签。
# print(ss.get_attribute('textContent'))        # 用textContent 和 innerText 只会得到文本内容，而不会包含 HTML 标签。
# print(ss.get_attribute('innerText'))          # innerText 不是 W3C DOM 的指定内容，FireFox不支持

# print("Visiting docker in practice site")
# mlinkselector = '.org-header a.meta-link'
# mlink = b.find_element_by_css_selector(mlinkselector)[0]
# mlink.click()

b.quit()
print('Done')
