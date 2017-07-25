﻿#include <stdafx.h>
#include "ConfigData.h"
#include "Utility\CommonConvert.h"
#include "Utility\Log\Log.h"
#include "Utility\CommonFunc.h"

CConfigData::CConfigData()
{

}

CConfigData::~CConfigData()
{

}

CConfigData* CConfigData::GetInstancePtr()
{
	static CConfigData _StaticMgr;

	return &_StaticMgr;
}

BOOL CConfigData::InitDataReader()
{
	m_vtDataFuncList.push_back(DataFuncNode("Data_Actor", &CConfigData::ReadActor));
	return TRUE;
}

BOOL CConfigData::ReadConfigData(std::string strDbFile)
{
	try
	{
		m_DBConnection.open(strDbFile.c_str());
	}
	catch(CppSQLite3Exception& e)  
	{  
		printf("%s",e.errorMessage());  
		return FALSE;
	}

	char szSql[1024]  = {0};
	for(std::vector<DataFuncNode>::iterator itor = m_vtDataFuncList.begin(); itor != m_vtDataFuncList.end(); itor++)
	{
		DataFuncNode dataNode = (*itor);
		sprintf(szSql, "select * from %s;",dataNode.m_strTbName);
		CppSQLite3Query Tabledatas = m_DBConnection.execQuery(szSql);
		(this->*dataNode.m_pDataFunc)(Tabledatas);
	}

	m_DBConnection.close();

	return TRUE;
}


BOOL CConfigData::ReadConstantValue(CppSQLite3Query &QueryData)
{
	while(QueryData.eof())
	{
		std::string strName = QueryData.getStringField("name");
		std::string strValue = QueryData.getStringField("value");

		if(strName == "xxxxx")
		{
			m_ConstantValue.xxxx = CommonConvert::StringToInt(strValue.c_str());
		}

		QueryData.nextRow();
	}

	return TRUE;
}

UINT64 CConfigData::GetMoneyMaxValue(UINT32 dwMoneyID)
{
	return 0;
}

UINT64 CConfigData::GetActoinMaxValue(UINT32 dwActionID)
{
	return 0;
}

UINT32 CConfigData::GetActoinUnitTime(UINT32 dwActionID)
{
	return 0;
}

BOOL CConfigData::ReadActor(CppSQLite3Query &QueryData)
{
	while(QueryData.eof())
	{
		StActor stValue;
		stValue.dwID = QueryData.getIntField("Id");
		m_mapActor.insert(std::make_pair(stValue.dwID, stValue));
		QueryData.nextRow();
	}

	return TRUE;
}

StActor* CConfigData::GetActorInfo(UINT32 dwActorID)
{
	std::map<UINT32, StActor>::iterator itor = m_mapActor.find(dwActorID);
	if(itor != m_mapActor.end())
	{
		return &itor->second;
	}

	return NULL;
}

BOOL CConfigData::ReadCopyBase(CppSQLite3Query &QueryData)
{
	while(QueryData.eof())
	{
		StCopyBase stValue;
		stValue.dwTypeID = QueryData.getIntField("Id");
		/*   stValue.dwBattleTimes = QueryData.getIntField("Id"); 
		stValue.dwCostActID = QueryData.getIntField("Id");
		stValue.dwCostActNum = QueryData.getIntField("Id");*/
        stValue.dwLogicType = QueryData.getIntField("Id");

		m_mapCopybase.insert(std::make_pair(stValue.dwTypeID, stValue));
		
		QueryData.nextRow();
	}

	return TRUE;
}

StCopyBase* CConfigData::GetCopyBaseInfo(UINT32 dwCopyType)
{
	std::map<UINT32, StCopyBase>::iterator itor = m_mapCopybase.find(dwCopyType);
	if(itor != m_mapCopybase.end())
	{
		return &itor->second;
	}

	return NULL;
}

BOOL CConfigData::ReadLanguage(CppSQLite3Query &QueryData)
{
	while(QueryData.eof())
	{
		StLocalString stValue;
		stValue.dwID = QueryData.getIntField("id");
        stValue.Language[0] = QueryData.getIntField("lang0");
        stValue.Language[1] = QueryData.getIntField("lang1");
        stValue.Language[2] = QueryData.getIntField("lang2");
        stValue.Language[3] = QueryData.getIntField("lang3");
        stValue.Language[4] = QueryData.getIntField("lang4");
        stValue.Language[5] = QueryData.getIntField("lang5");
        stValue.Language[6] = QueryData.getIntField("lang6");
        stValue.Language[7] = QueryData.getIntField("lang7");
        stValue.Language[8] = QueryData.getIntField("lang8");
        stValue.Language[9] = QueryData.getIntField("lang9");
        stValue.Language[10] = QueryData.getIntField("lang10");
        stValue.Language[11] = QueryData.getIntField("lang11");
        stValue.Language[12] = QueryData.getIntField("lang12");
        stValue.Language[13] = QueryData.getIntField("lang13");
        stValue.Language[14] = QueryData.getIntField("lang14");
		m_mapLocalString.insert(std::make_pair(stValue.dwID, stValue));
		QueryData.nextRow();
	}

	return TRUE;
}

std::string& CConfigData::GetLanguageText( UINT32 dwID, UINT32 dwLang )
{
    if((dwLang < 0)||(dwLang > 14))
    {
        return m_strNull;
    }

    std::map<UINT32, StLocalString>::iterator itor = m_mapLocalString.find(dwID);
    if(itor != m_mapLocalString.end())
    {
        return itor->second.Language[dwLang];
    }

    return m_strNull;
}

BOOL CConfigData::ReadAwardData(CppSQLite3Query &QueryData)
{
	while(QueryData.eof())
	{
		StAwardItem stValue;
		stValue.dwAwardID = QueryData.getIntField("awardid");
		std::string strFixDrop = QueryData.getStringField(2);
		std::string strRatioDrop = QueryData.getStringField(4);
		stValue.bDistinct = QueryData.getIntField(4);
		stValue.dwRatioCount  = QueryData.getIntField(4);
			//解析固定掉落
		if (strFixDrop != "NULL") 
		{
			std::vector<std::string> vtRet;
			CommonConvert::SpliteString(strFixDrop, ")(", vtRet);
			for(int i = 0; i < vtRet.size(); i++)
			{
				StDropItem item;
				ParseToDropItem(vtRet.at(i), item);
				stValue.FixItems.push_back(item);
			}
		}


		if (strRatioDrop != "NULL") 
		{
			UINT32 dwRatioBegin = 1;
			UINT32 dwTempValue = 0;
			std::vector<std::string> vtRet;
			CommonConvert::SpliteString(strFixDrop, ")(", vtRet);

			StDropItem item;
			for(int i = 0; i < vtRet.size(); i++)
			{
				ParseToDropItem(vtRet.at(i), item);
				stValue.RatioItems.push_back(item);

				dwTempValue = stValue.RatioItems[i].dwRatio;
				stValue.RatioItems[i].dwRatio = dwRatioBegin;
				dwRatioBegin += dwTempValue;
			}

	
			stValue.RatioItems.push_back(item);
			stValue.RatioItems[vtRet.size()].dwItemID = 0;
			stValue.RatioItems[vtRet.size()].dwRatio = 10000;
		}

		if ((stValue.FixItems.size() <= 0) && (stValue.RatioItems.size() <= 0)) 
		{
					
		}

		m_mapAwardItem.insert(std::make_pair(stValue.dwAwardID,stValue));

		QueryData.nextRow();
	}

	return TRUE;
}

BOOL CConfigData::ParseToDropItem(std::string strDrop, StDropItem &item)
{
	std::vector<std::string> vtRet;
	CommonConvert::SpliteString(strDrop, "|", vtRet);
	if (vtRet.size() < 3) 
	{
		return FALSE;
	}

	item.dwItemID = CommonConvert::StringToInt(vtRet[0].c_str());
	item.dwRatio = CommonConvert::StringToInt(vtRet[2].c_str());
	CommonConvert::SpliteString(vtRet[1], "|", vtRet);
	item.dwItemNum[0] =  CommonConvert::StringToInt(vtRet[0].c_str());
	item.dwItemNum[1] =  CommonConvert::StringToInt(vtRet[1].c_str());

	return TRUE;
}

BOOL CConfigData::GetAwardItemByIndex(UINT32 dwAwardID, UINT32 dwIndex, StItemData &ItemData)
{
	std::map<UINT32, StAwardItem>::iterator itor =  m_mapAwardItem.find(dwAwardID);
	if(itor == m_mapAwardItem.end())
	{
		return FALSE;
	}

	StAwardItem &AwardItem = itor->second;

	if (dwIndex >= AwardItem.FixItems.size()) 
	{
		CLog::GetInstancePtr()->LogError("GetItemByIndex Error: Invalid index :%d", dwIndex);
		return FALSE;
	}

	ItemData.dwItemID = AwardItem.FixItems[dwIndex].dwItemID;
	ItemData.dwItemNum = AwardItem.FixItems[dwIndex].dwItemNum[0];
	return TRUE;
}

BOOL CConfigData::GetItemsFromAwardID(UINT32 dwAwardID, std::vector<StItemData> &vtItemList)
{
	std::map<UINT32, StAwardItem>::iterator itor =  m_mapAwardItem.find(dwAwardID);
	if(itor == m_mapAwardItem.end())
	{
		return FALSE;
	}

	StAwardItem &AwardItem = itor->second;

	StItemData tempItem;

		for (int i = 0; i < AwardItem.FixItems.size(); i++ )
		{
			tempItem.dwItemID = AwardItem.FixItems[i].dwItemID;
				if (AwardItem.FixItems[i].dwItemNum[0] == AwardItem.FixItems[i].dwItemNum[1]) 
				{
					tempItem.dwItemNum = AwardItem.FixItems[i].dwItemNum[0];
				} 
				else 
				{
					tempItem.dwItemNum = AwardItem.FixItems[i].dwItemNum[0] +CommonFunc::GetRandNum(0)%(AwardItem.FixItems[i].dwItemNum[1]-AwardItem.FixItems[i].dwItemNum[0]+1);
				}

				if (tempItem.dwItemNum > 0) 
				{
					vtItemList.push_back(tempItem);
				}
		}
	


			for (int  cycle = 0; cycle < AwardItem.dwRatioCount; cycle++ )
			{
				UINT32 dwRandValue = CommonFunc::GetRandNum(0);
			    for (int i = 0; i < AwardItem.RatioItems.size() - 1; i++)
				{
				   if ((dwRandValue >= AwardItem.RatioItems[i].dwRatio) && (dwRandValue < AwardItem.RatioItems[i+1].dwRatio)) 
				   {
					   tempItem.dwItemID = AwardItem.RatioItems[i].dwItemID;
					   if (AwardItem.RatioItems[i].dwItemNum[1] == AwardItem.RatioItems[i].dwItemNum[0]) 
					   {
						   tempItem.dwItemNum = AwardItem.RatioItems[i].dwItemNum[0];
					   } 
					   else 
					   {
						   tempItem.dwItemNum = AwardItem.RatioItems[i].dwItemNum[0] + CommonFunc::GetRandNum(0)%(AwardItem.RatioItems[i].dwItemNum[1]-AwardItem.RatioItems[i].dwItemNum[0]+1);
					   }

					   if (tempItem.dwItemNum > 0) 
					   {
						   vtItemList.push_back(tempItem);
					   }
				   }
			   }
			}
		
		return TRUE;
}

BOOL CConfigData::GetItemsAwardIDTimes(UINT32 dwAwardID, UINT32 dwTimes, std::vector<StItemData> &vtItemList)
{
	std::map<UINT32, StAwardItem>::iterator itor =  m_mapAwardItem.find(dwAwardID);
	if(itor == m_mapAwardItem.end())
	{
		return FALSE;
	}

	StAwardItem &AwardItem = itor->second;

	StItemData tempItem;

	for (int i = 0; i < AwardItem.FixItems.size(); i++ )
	{
		tempItem.dwItemID = AwardItem.FixItems[i].dwItemID;
		if (AwardItem.FixItems[i].dwItemNum[0] == AwardItem.FixItems[i].dwItemNum[1]) 
		{
			tempItem.dwItemNum = AwardItem.FixItems[i].dwItemNum[0];
		} 
		else 
		{
			tempItem.dwItemNum = AwardItem.FixItems[i].dwItemNum[0] +CommonFunc::GetRandNum(0)%(AwardItem.FixItems[i].dwItemNum[1]-AwardItem.FixItems[i].dwItemNum[0]+1);
		}

		if (tempItem.dwItemNum > 0) 
		{
			vtItemList.push_back(tempItem);
		}
	}



	for (int  cycle = 0; cycle < AwardItem.dwRatioCount*dwTimes; cycle++ )
	{
		UINT32 dwRandValue = CommonFunc::GetRandNum(0);
		for (int i = 0; i < AwardItem.RatioItems.size() - 1; i++)
		{
			if ((dwRandValue >= AwardItem.RatioItems[i].dwRatio) && (dwRandValue < AwardItem.RatioItems[i+1].dwRatio)) 
			{
				tempItem.dwItemID = AwardItem.RatioItems[i].dwItemID;
				if (AwardItem.RatioItems[i].dwItemNum[1] == AwardItem.RatioItems[i].dwItemNum[0]) 
				{
					tempItem.dwItemNum = AwardItem.RatioItems[i].dwItemNum[0];
				} 
				else 
				{
					tempItem.dwItemNum = AwardItem.RatioItems[i].dwItemNum[0] + CommonFunc::GetRandNum(0)%(AwardItem.RatioItems[i].dwItemNum[1]-AwardItem.RatioItems[i].dwItemNum[0]+1);
				}

				if (tempItem.dwItemNum > 0) 
				{
					vtItemList.push_back(tempItem);
				}
			}
		}
	}
		return TRUE;
}