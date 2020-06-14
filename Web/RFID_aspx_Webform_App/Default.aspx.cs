
using System;
using System.Web.UI;
using System.Data;
using System.Data.SqlClient;
using Microsoft.ApplicationBlocks.Data;
using CustomControls;

namespace RFID
{
    public partial class _Default : Page
    {

        public string ThisUID()
        {
            return common.SafeParam(Request.QueryString["UID"]);
        }
        public string ThisMAC()
        {
            return common.SafeParam(Request.QueryString["MAC"]);
        }
        public string ThisMSG()
        {
            return common.SafeParam(Request.QueryString["MSG"]);
        }
        public int MyProperty { get; set; }

        private string _ResultMessage = "";
        public string ResultMessage
        {
            get {
                return _ResultMessage; 
            }
        }
        //{
        //    string p = ThisUID();
        //    return p;
        //}

        public void SaveData()
        {
            SqlParameter[] sqlParams = new SqlParameter[]
            {
                    new SqlParameter("@CARDSN",ThisUID()),
                    new SqlParameter("@MAC",ThisMAC()),
                    new SqlParameter("@MSG",ThisMSG()),
                    new SqlParameter("@echo_output","true"),
                    new SqlParameter("@debug_status","0"),
            };
            string strSQL = "dbo.proc_add_RFID_UID_log";
            try
            {
                string thisConnectionString = common.strConnectionString("Tracker");
                _ResultMessage  = SqlHelper.ExecuteScalar(thisConnectionString, CommandType.StoredProcedure, strSQL, sqlParams).ToString();
            }
            catch (Exception ex)
            {
                _ResultMessage = ex.Message;
            }
        }
        protected void Page_Load(object sender, EventArgs e)
        {
            string FoundUID = ThisUID();
            if ((FoundUID != null) && (FoundUID.Length == 8))
            {
                try
                {
                    SaveData();
                }
                catch (Exception ex)
                {
                    _ResultMessage = ex.Message;
                }
            }
            else
            {
                _ResultMessage = "Error: 404";
            }
        }
    }
}