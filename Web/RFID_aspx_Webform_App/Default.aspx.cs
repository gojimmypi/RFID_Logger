
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

using System.Data;
using System.Data.SqlClient;
using Microsoft.ApplicationBlocks.Data;
using CustomControls;
using System.Web.Optimization;
using System.Xml.Serialization;
using System.IO;

namespace RFID
{
    public partial class _Default : Page
    {

        public string ThisUID()
        {
            return common.SafeParam(Request.QueryString["UID"]);
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
                    new SqlParameter("@device_id","test"),
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