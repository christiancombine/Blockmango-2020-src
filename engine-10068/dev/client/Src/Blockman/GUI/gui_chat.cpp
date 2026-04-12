#include "gui_chat.h"
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIEditBox.h"
#include "GUI/RootGuiLayout.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Util/LanguageKey.h"
#include "Util/ClientCmdMgr.h"
#include "Setting/FilterWord.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Render/RenderEntity.h"
#include "Actor/ActorObject.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Render/RenderGlobal.h"
#include "Object/Root.h"
#include "Tessolator/TessManager.h"
#include "Anim/SkeletonManager.h"
#include "Model/MeshManager.h"
#include "Anim/AnimManager.h"
#include "UI/GUIImagesetManager.h"
#include "Memory/MemTracker.h"
#include "cWorld/Blockman.h"
#include "World/Section.h"
#include "cWorld/WorldClient.h"

namespace BLOCKMAN {
	gui_chat::gui_chat() :
		gui_layout("Chat.json")
	{
	}

	gui_chat::~gui_chat()
	{
        m_subscriptionGuard.unsubscribeAll();
	}

	void gui_chat::onLoad()
	{
		getWindow<GUIButton>("Chat-BtnBack")->subscribeEvent(EventButtonClick, std::bind(&gui_chat::onBackClick, this, std::placeholders::_1));
		getWindow<GUIButton>("Chat-BtnSend")->subscribeEvent(EventButtonClick, std::bind(&gui_chat::onSendClick, this, std::placeholders::_1));

		m_InputBox = getWindow<GUIEditBox>("Chat-Input-Box");
		m_InputBox->subscribeEvent(EventWindowTextChanged, std::bind(&gui_chat::onInputChanged, this, std::placeholders::_1));

		m_chatMessageListBox = getWindow<GUIListBox>("Chat-Content");
		m_subscriptionGuard.add(ChatMessageReceiveEvent::subscribe(std::bind(&gui_chat::addChatMessage, this, std::placeholders::_1, std::placeholders::_2)));
		m_chatMessageListBox->SetAllowScroll(true);
		m_chatMessageListBox->SetTouchable(true);
		m_InputBox->SetBlinkshow(true);
	}

	bool gui_chat::onInputChanged(const EventArgs & events)
	{
		std::string text = m_InputBox->GetText().c_str();
		FilterWord::replace(text);
		m_InputBox->SetText(text.c_str());
		return true;
	}

	bool gui_chat::onBackClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		//playSonud("random.click");
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_chat::onSendClick(const EventArgs & events)
	{
		if (m_InputBox->GetText().length() > 0)
		{
			// client command for debug rendering.
			String msg = m_InputBox->GetText().c_str();

			if (msg.find("renderdebug") == 0)
			{
				StringArray args = StringUtil::Split(msg, " ");
				if (args.size() == 2)
				{
					int queueId = StringUtil::ParseInt(args[1]);
					RenderQueue* queue = SceneManager::Instance()->getRenderQueueByIndex(queueId);
					if (queue != nullptr)
					{
						bool enable = queue->isEnable();
						enable = !enable;
						if (enable)
							Root::Instance()->enableRenderQueue(queueId);
						else
							Root::Instance()->disableRenderQueue(queueId);
					}
				}
			}
			else if (msg.find("setblocks") == 0)
			{
				// set all around player's 5*5 chunks data to all dirty!
				Vector3 playerPos = Blockman::Instance()->m_pPlayer->position;
				Vector3i worldPos(playerPos.x, playerPos.y - 2, playerPos.z);
				BLOCKMAN::BlockModifys modifies;

				int chunkX = worldPos.x >> 4;
				int chunkZ = worldPos.z >> 4;
				for (int iz = -2; iz <= 2; ++iz)
				{
					for (int ix = -2; ix <= 2; ++ix)
					{
						modifies.clear();
						int worldbeginx = (ix + chunkX) * 16;
						int worldbeginz = (iz + chunkZ) * 16;
						for (int cy = 0; cy < worldPos.y; ++cy)
						{
							for (int cx = 0; cx < 16; ++cx)
							{
								for (int cz = 0; cz < 16; ++cz)
								{
									modifies.push_back({ {worldbeginx + cx,cy, worldbeginz + cz}, 7, 0 });
								}
							}
						}
						Blockman::Instance()->m_pWorld->setBlocks(chunkX + ix, chunkZ + iz, modifies);
					}
				}
			}
			else if (msg.find("rendertoggle") == 0)
			{
				StringArray args = StringUtil::Split(msg, " ");
				if (args.size() == 2)
				{
					int queueId = StringUtil::ParseInt(args[1]);
					bool toggled = LORD::TessManager::Instance()->isRenderQueueToggled(queueId);
					toggled = !toggled;
					LORD::TessManager::Instance()->toggleRenderQueue(queueId, toggled);
				}
			}
			else if (msg.find("reportmem") == 0) // report all the memory.
			{
				/*TextureManager::Instance()->reportMemory("TextureManager");
				SkeletonManager::Instance()->reportMemory("SkeletonManager");
				AnimManager::Instance()->reportMemory("AnimManager");
				MeshManager::Instance()->reportMemory("MeshManager");
				GUIImagesetManager::Instance()->reportMemory("GUIImagesetManager");*/
#ifdef LORD_MEMORY_TRACKER
				MemTracker::get().reportLeaks();
#endif
			}
			else if (msg.find("refresh") == 0)
			{
				// refresh all the sectionRenders around player's 5*5*5 sections.
				Vector3 playerPos = Blockman::Instance()->m_pPlayer->position;
				Vector3i playerSectionID(playerPos.x / 16, (playerPos.y - 2)/16, playerPos.z / 16);
				for (int ix = -5; ix <= 5; ++ix)
				{
					for (int iy = -5; iy <= 5; ++iy)
					{
						for (int iz = -5; iz <= 5; ++iz)
						{
							Vector3i dirtySectionID = playerSectionID + Vector3i(ix, iy, iz);
							if (dirtySectionID.y < 0 || dirtySectionID.y >= 16)
								continue;
							Blockman::Instance()->m_pWorld->markSectionForUpdate(dirtySectionID);
						}
					}
				}
			}
			
			if (!ClientCmdMgr::consumeCmd(m_InputBox->GetText().c_str())) {
				std::string msg = m_InputBox->GetText().c_str();
				FilterWord::replace(msg);
				if (msg.length() >= 100)
				{
					msg = msg.substr(0, 98);
				}
				ClientNetwork::Instance()->getSender()->sendChatString(msg.c_str());
			}
			m_InputBox->SetText("");
			RootGuiLayout::Instance()->showMainControl();
			//playSonud("random.click");
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_chat::addChatMessage(const String&  message, bool isChatInformation)
	{
		m_messageCount = m_messageCount + 1;
		String strTextName = StringUtil::Format("Chat-Content-List-item-%d", m_messageCount).c_str();
		GUIStaticText* pStaticText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextName.c_str());
		pStaticText->SetText(message.c_str());
		pStaticText->SetWidth(UDim(1, 0));
		pStaticText->SetHeight(UDim(0, 30));
		pStaticText->SetTouchable(false);
		pStaticText->SetTextVertAlign(VA_CENTRE);
		pStaticText->SetYPosition(UDim(0, 0));
		pStaticText->SetTextScale(1.2f);
		m_chatMessageListBox->AddItem(pStaticText, true);
		scrollBottom();
		return true;
	}

	void gui_chat::scrollBottom()
	{
		if (m_chatMessageListBox->getContainerWindow() && m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y < 0)
		{
			m_chatMessageListBox->SetScrollOffset(m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y);
		}
	}
}
