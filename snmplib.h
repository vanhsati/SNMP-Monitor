#ifndef SNMPLIB_H
#define SNMPLIB_H
#include <QString>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string.h>

class SNMPLib
{
public:
    SNMPLib();
    bool init(QString IP, QString community);
    bool sendOID(QString oid,QString *value);
    void closeSession();

protected:
    netsnmp_session session, *ss;
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;

    oid anOID[MAX_OID_LEN];
    size_t anOID_len;

    netsnmp_variable_list *vars;
};

#endif // SNMPLIB_H
