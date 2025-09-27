#if !defined(KEY_H)
#define KEY_H

#pragma warning(disable:4786)  // don't warn about browse name overflow.

#include <map>

#include <vector.h>
#include <vertex.h>
#include <quaternion.h>
#include "z3dmath.h"

typedef std::map< unsigned long, vector3, std::less<unsigned long> > VectorKeyList;
typedef std::map< unsigned long, quaternion, std::less<unsigned long> > QuaternionKeyList;
typedef std::map< unsigned long, float, std::less<unsigned long> > FloatKeyList;
typedef std::map< unsigned long, color, std::less<unsigned long> > ColorKeyList;

template <typename KeyList> class CKeyList
{
	protected:
		KeyList m_lstKey;
		typename KeyList::iterator Lower, Upper;
		unsigned long dwLower, dwUpper;
	
	public:
		CKeyList(void) { }
		~CKeyList(void) { }

		typename KeyList::iterator Begin(void) { return m_lstKey.begin(); }
		typename KeyList::iterator End(void) { return m_lstKey.end(); }
		template <typename NewData> void SetKey(unsigned long dwFrame, NewData &Data) { m_lstKey[dwFrame] = Data; }
		template <typename NewData> BOOL Find(unsigned long dwFrame, NewData &Data)
		{
			if(m_lstKey.empty()) return FALSE;

			Lower = m_lstKey.find(dwFrame);
			if(Lower == m_lstKey.end())
			{
				Lower = m_lstKey.lower_bound(dwFrame);
				if(Lower != m_lstKey.begin()) { Lower--; }
				Data = ((*Lower).second);

				return FALSE;
			}
			Data = ((*Lower).second);
			return TRUE;
		}
		BOOL Find(unsigned long dwFrame)
		{
			if(m_lstKey.empty()) return FALSE;

			Lower = m_lstKey.find(dwFrame);
			if(Lower == m_lstKey.end()) return FALSE; else return TRUE;
		}
		void Erase(unsigned long dwFrame)
		{
			Lower = m_lstKey.find(dwFrame);
			m_lstKey.erase(Lower);
		}
		template <typename DataType> void Load(FILE *fp, DataType &Data)
		{
			unsigned short frame;
			dwLower = 0;
			fread(&dwLower, 2, 1, fp);
			for(dwUpper = 0; dwUpper < dwLower; dwUpper++)
			{
				fread(&frame, 2, 1, fp);
				fread(&Data, sizeof(Data), 1, fp);
				m_lstKey[frame] = Data;
			}
		}
		void Save(FILE *fp)
		{
			dwLower = m_lstKey.size();
			fwrite(&dwLower, 2, 1, fp);
			for(Lower = m_lstKey.begin(); Lower != m_lstKey.end(); Lower++)
			{
				fwrite(&((*Lower).first), 2, 1, fp);
				fwrite(&((*Lower).second), sizeof((*Lower).second), 1, fp);
			}
		}
		BOOL InterpolationQ(float fFrame, quaternion &Data)
		{
			quaternion rdLower, rdUpper;

			if(m_lstKey.empty()) return FALSE;

			Lower = Upper = m_lstKey.lower_bound(ceilf(fFrame));
			if(Upper == m_lstKey.begin()) { Upper = Lower; Upper++; } else { Lower--; }

			dwLower = (*Lower).first;
			rdLower = (*Lower).second;
			if(Upper != m_lstKey.end())
			{
				dwUpper = (*Upper).first;
				rdUpper = (*Upper).second;
				z3d::QuaternionSlerp(Data, rdLower, rdUpper, 1.0f / (dwUpper - dwLower) * (fFrame - dwLower));
			} else
			{
				Data = rdLower;
			}

			return TRUE;
		}
		BOOL InterpolationC(float fFrame, color &Data)
		{
			color rdLower, rdUpper;

			if(m_lstKey.empty()) return FALSE;

			Lower = Upper = m_lstKey.lower_bound(ceilf(fFrame));
			if(Upper == m_lstKey.begin()) { Upper = Lower; Upper++; } else { Lower--; }

			dwLower = (*Lower).first;
			rdLower = (*Lower).second;
			if(Upper != m_lstKey.end())
			{
				dwUpper = (*Upper).first;
				rdUpper = (*Upper).second;
				Data = color::Interpolation(rdLower, rdUpper, 1.0f / (dwUpper - dwLower) * (fFrame - dwLower));
			} else
			{
				Data = rdLower;
			}

			return TRUE;
		}
		template <typename RetData> BOOL Interpolation(float fFrame, RetData &Data)
		{
			RetData rdLower, rdUpper;

			if(m_lstKey.empty()) return FALSE;

			Lower = Upper = m_lstKey.lower_bound(ceilf(fFrame));
			if(Upper == m_lstKey.begin()) { Upper = Lower; Upper++; } else { Lower--; }

			dwLower = (*Lower).first;
			rdLower = (*Lower).second;
			if(Upper != m_lstKey.end())
			{
				dwUpper = (*Upper).first;
				rdUpper = (*Upper).second;
				Data = (((rdUpper - rdLower) / (dwUpper - dwLower)) * (fFrame - dwLower)) + rdLower;
			} else
			{
				Data = rdLower;
			}

			return TRUE;
		}
};

template <typename KeyList, class Data>
inline void SetKey(unsigned long dwFrame, KeyList &lstKey, Data NewData)
{
	lstKey.SetKey(dwFrame, NewData);
}

#endif // !defined(KEY_H)
