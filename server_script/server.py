"""
EECS 149/249 Project demo
Email Notifier - Server

@author: Antonio Iannopollo
"""

import urllib2
import xml.etree.ElementTree as ET
import time

GMAIL_USER = 'username'
GMAIL_PASSWORD = 'password'
LABEL = 'label'
FILEPATH = '/path/to/file'

def get_unread_msgs(user, passwd, label = ""):
    auth_handler = urllib2.HTTPBasicAuthHandler()
    auth_handler.add_password(
        realm='New mail feed',
        uri='https://mail.google.com',
        user='%s@gmail.com' % user,
        passwd=passwd)
    opener = urllib2.build_opener(auth_handler)
    urllib2.install_opener(opener)
    feed = urllib2.urlopen('https://mail.google.com/mail/feed/atom/' + label)
    return feed.read()

def count_unread_emails(xml_text):
    root = ET.fromstring(xml_text)
    if root[2] == None:
        raise NotValidXMLError('count')
    count = root[2].text
    return count

def NotValidXMLError(Exception):
    pass


if __name__ == '__main__':

    print 'EECS 149/249 email notifier - server'

    while True:
        xml_text = get_unread_msgs(GMAIL_USER, GMAIL_PASSWORD, LABEL)

        count = count_unread_emails(xml_text)

        file = open(FILEPATH, 'w')
        file.write(count)
        file.close()

        time.sleep(5)

