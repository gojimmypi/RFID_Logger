CREATE TABLE dbo.RFID_UID_log
(
    RFID_UID_log_ID    int          IDENTITY,
    CARDSN             varchar(20)  COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL,
    device_id          varchar(20)  COLLATE SQL_Latin1_General_CP1_CI_AS DEFAULT '' NOT NULL,
    create_dt          datetime     DEFAULT getdate() NOT NULL,
    create_suser_sname varchar(128) COLLATE SQL_Latin1_General_CP1_CI_AS DEFAULT suser_sname() NOT NULL,
    create_app_user_id varchar(128) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL,
    modify_dt          datetime     DEFAULT getdate() NOT NULL,
    modify_suser_sname varchar(128) COLLATE SQL_Latin1_General_CP1_CI_AS DEFAULT suser_sname() NOT NULL,
    modify_app_user_id varchar(128) COLLATE SQL_Latin1_General_CP1_CI_AS DEFAULT '' NOT NULL,
    CONSTRAINT PK_RFID_UID_log
    PRIMARY KEY NONCLUSTERED (RFID_UID_log_ID)
)
go
 
