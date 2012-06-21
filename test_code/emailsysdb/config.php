<?php
/* ?~B?件?~X~_?~H~W中?~H~F表?~X?~B??~Z~D修?~T??~W??~W??~W段 */
$_SCONFIG['mail_queue_last_modified_date_field'] = array(
    'instant_message_queue' => 'send_date',
    'msg_cs_queue'          => 'send_date',
    'msg_order_queue'       => 'send_date',
    'reco_mail_queue'       => 'last_modified_date',
);

$_SC['Msg_queue'] = array(
        'dsn' => 'dblib:host=msgqdb.idc3;dbname=Msg_queue',
        'user' => 'writeuser',
        'psw' => 'wt@msg_queue55',
        );
// job_log
$_SC['joblog'] = array(
        'dsn' => 'mysql:host=joblogdb.idc2;dbname=joblog',
        'user' => 'joblog_sir',
        'psw' => 'r1sg0lb0j',
        );
// EmailSystem
$_SC['EmailSystemDB'] = array(
        'dsn' => 'mysql:host=myemailsysdb.idc3;dbname=EmailSystemDB',
        'user' => 'emailsysdb_sir',
        'psw' => 'r1sbdsysl14m3',
        );
