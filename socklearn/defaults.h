#ifndef DEFAULTS_H
#define DEFAULTS_H


#define DEFAULT_PORT "27015"// Default Port of server
#define BUFF_LEN 512


#define MSG_SENT "500\0" //Message sent status code 
#define MSG_SENT_LEN 4L
#define MSG_DELVD "502\0"//Message delivered status code

//@brief representing phone number of client in buff 
#define SOCK_ID_LEN 10

#define CLIENT_HANDLE_FAIL  302

#define TIMEOUT_IN_SECONDS 3


#endif

//MSG_SENT_PREF
//MSG_DELVD_PREF
//ECHO_PREF
//MSG_FRWD_PREF
