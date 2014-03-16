#include "snmplib.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string.h>
#include <QDebug>
#include <QString>
#include <QFile>

SNMPLib::SNMPLib()
{}

bool SNMPLib::init(QString IP,QString community)
{
    /*
     * Initialize the SNMP library
     */
    init_snmp("snmpdemoapp");

    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    snmp_sess_init( &session );                   /* set up defaults */
    session.peername = IP.toLocal8Bit().data();

    qDebug() << session.peername;

    /* set up the authentication parameters for talking to the server */

    /* we'll use the insecure (but simplier) SNMPv1 */

    /* set the SNMP version number */
    session.version = SNMP_VERSION_1;

    /* set the SNMPv1 community name used for authentication */
    session.community = (u_char*) community.toLocal8Bit().data();
    session.community_len = strlen((char*) session.community);

    /* SNMPv1 */

    /*
     * Open the session
     */
    SOCK_STARTUP;
    ss = snmp_open(&session);                     /* establish the session */
    if (!ss) {
        snmp_sess_perror("ack", &session);
        SOCK_CLEANUP;
        return false;
        exit(1);
    }
//    if (!snmp_parse_oid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len)) {
//        return false;
//    }
    return true;
}

bool SNMPLib::sendOID(QString oid,QString *value)
{
int status;

    /*
     * Create the PDU for the data for our request.
     *   1) We're going to GET the system.sysDescr.0 node.
     */
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    anOID_len = MAX_OID_LEN;
    if (!snmp_parse_oid(oid.toLocal8Bit().data(), anOID, &anOID_len)){
//    if (!read_objid(oid.toLocal8Bit().data(),anOID,&anOID_len)){
        snmp_perror(oid.toLocal8Bit().data());
        SOCK_CLEANUP;
        exit(1);
    }

    snmp_add_null_var(pdu, anOID, anOID_len);

    /*
     * Send the Request out.
     */
    status = snmp_synch_response(ss, pdu, &response);

    /*
     * Process the response.
     */
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
      /*
       * SUCCESS: Print the result variables
       */

        FILE *fp;
        fp=fopen("temp.txt", "w+");
        for(vars = response->variables; vars; vars = vars->next_variable)
            fprint_variable(fp,vars->name, vars->name_length, vars);
        fclose(fp);

        QFile file("temp.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QString line = in.readLine();
        line.remove(0,line.indexOf("=")+1);
        *value = line.remove(0,line.indexOf(":")+1).trimmed();
        file.close();

    } else {
      /*
       * FAILURE: print what went wrong!
       */

      if (status == STAT_SUCCESS)
        fprintf(stderr, "Error in packet\nReason: %s\n",
                snmp_errstring(response->errstat));
      else if (status == STAT_TIMEOUT){
        fprintf(stderr, "Timeout: No response from %s.\n", session.peername);
        return false;
      }
      else
        snmp_sess_perror("snmpdemoapp", ss);
    }
    return true;
}

void SNMPLib::closeSession()
{
    /*
     * Clean up:
     *  1) free the response.
     *  2) close the session.
     */
    if (response)
        snmp_free_pdu(response);
        snmp_close(ss);

    SOCK_CLEANUP;
}
