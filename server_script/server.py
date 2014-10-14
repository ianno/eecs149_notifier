"""
EECS 149/249 Project demo
Email Notifier - Server

This application constantly polls the gmail RSS atom server to check for unread
emails with a specific label (in this case [EECS 149/249]).
The script uses the 'urllib2' library to connect to the server using a secure
protocol and then write the number of unread emails in a text file.

For the purpose of this demo, the text file is written in a public folder (personal
 website folder) so that the mbed platform is able to remotely connect to the
 website and read the uploaded value.

@author: Antonio Iannopollo
"""

import urllib2
import xml.etree.ElementTree as ET
import time

#module variables
GMAIL_USER = 'username'
GMAIL_PASSWORD = 'password'
LABEL = '%5Beecs-149-249%5D'
FILEPATH = '/path/to/file'

def get_unread_msgs(user, passwd, label = ""):
    """
    This function establishes a secure connection to the Gmail RSS service and
    returns the XML text associated with the specified Gmail label.
    Follow the documentation for urllib2 for further details.
    """
    auth_handler = urllib2.HTTPBasicAuthHandler()
    auth_handler.add_password(
        realm=None,
        uri='https://mail.google.com',
        user='%s@gmail.com' % user,
        passwd=passwd)
    opener = urllib2.build_opener(auth_handler)
    urllib2.install_opener(opener)
    feed = urllib2.urlopen('https://mail.google.com/mail/feed/atom/' + label)
    return feed.read()

def count_unread_emails(xml_text):
    """
    This method extracts the number of unread emails from the XML string fetched
    from the Gmail server.
    We are interested in the 'fullcount' field of the XML file.
    Since the structure of the XML text is always the same, we can access the
    field directly with its index (2 from the document root in this case)
    """
    root = ET.fromstring(xml_text)
    if root[2] == None:
        raise NotValidXMLError('count')
    count = root[2].text
    return count

class NotValidXMLError(Exception):
    """
    An exception to be raised if the structure of the XML text is not what we
    expect
    """
    pass


if __name__ == '__main__':

    print 'EECS 149/249 email notifier - server'

    while True:
#continuosly fetch the number of emails and update the text file, then sleep
#for 5 seconds
        XML_TEXT = get_unread_msgs(GMAIL_USER, GMAIL_PASSWORD, LABEL)

        COUNT = count_unread_emails(XML_TEXT)

        TXT_FILE = open(FILEPATH, 'w')
        TXT_FILE.write(COUNT)
        TXT_FILE.close()

        time.sleep(5)

