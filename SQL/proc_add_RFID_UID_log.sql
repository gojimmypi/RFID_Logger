create proc dbo.proc_add_RFID_UID_log
  @CARDSN    varchar(20) = Null,
  @MSG       varchar(255) = '',
  @MAC       varchar(20) = '',
  @IP        varchar(20) = '',
  @device_id varchar(20) = '',
  @echo_output  as varchar(12) = Null,
  @debug_status as varchar(12) = Null
AS
BEGIN
  /*
  ** Procedure: proc_add_RFID_UID_log
  **
  ** Params
  **
  ** Author: gojimmypi
  **
  ** Description:
  ** -------------------------------------------------------------------------------------------------------------------------------
  **
  **
  ** -------------------------------------------------------------------------------------------------------------------------------
  ** Mods:
  **
  ** Date      Version  By             Description
  ** -------------------------------------------------------------------------------------------------------------------------------
  ** 01JUN20    1.0     gojimmypi      initial code
  */
  SET NOCOUNT ON
  Declare @strRes varchar(64)
  Declare @ID int = 0

  INSERT INTO Tracker.dbo.RFID_UID_log
      (CARDSN,
       MSG,
       MAC,
       IP,
       device_id)
  VALUES
      (@CARDSN,
       @MSG,
       @MAC,
       @IP,
       @device_id )

  SET @ID = SCOPE_IDENTITY();
  
  /*
  ** --------------------------------------------------------------------------
  ** done. echo results if requested
  ** --------------------------------------------------------------------------
  */
  If (@echo_output = 'Y' or @echo_output='ON' or @echo_output='TRUE') Begin
    SELECT @ID as ID
  End

  RETURNEND
go
IF OBJECT_ID(N'dbo.proc_add_RFID_UID_log') IS NOT NULL
    PRINT N'<<< CREATED PROCEDURE dbo.proc_add_RFID_UID_log >>>'
ELSE
    PRINT N'<<< FAILED CREATING PROCEDURE dbo.proc_add_RFID_UID_log >>>'
go
GRANT EXECUTE ON dbo.proc_add_RFID_UID_log TO [your service account role]
go

