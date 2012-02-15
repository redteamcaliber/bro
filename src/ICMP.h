// See the file "COPYING" in the main distribution directory for copyright.

#ifndef icmp_h
#define icmp_h

#include "Analyzer.h"

typedef enum {
	ICMP_INACTIVE,	// no packet seen
	ICMP_ACTIVE,	// packets seen
} ICMP_EndpointState;

// We do not have an PIA for ICMP (yet) and therefore derive from
// RuleMatcherState to perform our own matching.
class ICMP_Analyzer : public TransportLayerAnalyzer {
public:
	ICMP_Analyzer(Connection* conn);

	virtual void UpdateConnVal(RecordVal *conn_val);

	static Analyzer* InstantiateAnalyzer(Connection* conn)
		{ return new ICMP_Analyzer(conn); }

	static bool Available()	{ return true; }

protected:
	ICMP_Analyzer()	{ }
	ICMP_Analyzer(AnalyzerTag::Tag tag, Connection* conn);

	virtual void Done();
	virtual void DeliverPacket(int len, const u_char* data, bool orig,
					int seq, const IP_Hdr* ip, int caplen);
	virtual bool IsReuse(double t, const u_char* pkt);
	virtual unsigned int MemoryAllocation() const;

	void ICMPEvent(EventHandlerPtr f, const struct icmp* icmpp, int len, int icmpv6);

	void Echo(double t, const struct icmp* icmpp, int len,
			 int caplen, const u_char*& data, const IP_Hdr* ip_hdr);
	void Context(double t, const struct icmp* icmpp, int len,
			 int caplen, const u_char*& data, const IP_Hdr* ip_hdr);
	void Router(double t, const struct icmp* icmpp, int len,
			 int caplen, const u_char*& data, const IP_Hdr* ip_hdr);

	void Describe(ODesc* d) const;

	RecordVal* BuildICMPVal(const struct icmp* icmpp, int len, int icmpv6);

	void NextICMP4(double t, const struct icmp* icmpp, int len, int caplen,
			const u_char*& data, const IP_Hdr* ip_hdr );

	RecordVal* ExtractICMP4Context(int len, const u_char*& data);

	void Context4(double t, const struct icmp* icmpp, int len, int caplen,
			const u_char*& data, const IP_Hdr* ip_hdr);

	TransportProto GetContextProtocol(const IP_Hdr* ip_hdr, uint32* src_port,
			uint32* dst_port);

#ifdef BROv6
	void NextICMP6(double t, const struct icmp* icmpp, int len, int caplen,
			const u_char*& data, const IP_Hdr* ip_hdr );

	RecordVal* ExtractICMP6Context(int len, const u_char*& data);

	void Context6(double t, const struct icmp* icmpp, int len, int caplen,
			const u_char*& data, const IP_Hdr* ip_hdr);
#endif

	RecordVal* icmp_conn_val;
	int request_len, reply_len;

	RuleMatcherState matcher_state;

private:
	void UpdateEndpointVal(RecordVal* endp, int is_orig);
};

// Returns the counterpart type to the given type (e.g., the counterpart
// to ICMP_ECHOREPLY is ICMP_ECHO).
extern int ICMP4_counterpart(int icmp_type, int icmp_code, bool& is_one_way);
extern int ICMP6_counterpart(int icmp_type, int icmp_code, bool& is_one_way);

#endif
