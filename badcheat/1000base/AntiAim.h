#pragma once

class AntiAim {
public:
	void Move(CUserCmd* pCmd, bool *bSendPacket);
	void FakeLag(bool *bSendPacket);
private:
	void AtTargets(CUserCmd* pCmd);
	void FreeStanding(CUserCmd* pCmd, bool bSendPacket);
	void FakeWalk(CUserCmd* pCmd, bool auto_stop, bool bSendPacket);

	bool send_packet = false;
}; extern AntiAim* g_AntiAim;