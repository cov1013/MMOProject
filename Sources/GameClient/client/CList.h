//////////////////////////////////////////////////////////////
// STL List 만들기
//
//////////////////////////////////////////////////////////////
#pragma once
#include <memory.h>

template <typename T>
class CList
{
public:
	//-------------------------------------------------------------
	// 생성자
	//-------------------------------------------------------------
	CList()
	{
		//-------------------------------------------------------------
		// Head, Tail 초기화
		//-------------------------------------------------------------
		memset(&mHead, 0, sizeof(Node));
		memset(&mTail, 0, sizeof(Node));

		//-------------------------------------------------------------
		// Head와 Tail을 이어준다.
		//-------------------------------------------------------------
		mHead.Next = &mTail;
		mTail.Prev = &mHead;
		mSize = 0;
	}

	//-------------------------------------------------------------
	// 소멸자
	//-------------------------------------------------------------
	~CList()
	{
	}

private:
	struct Node
	{
		T Data;		// 템플릿 데이터를 담을 실직적인 데이터
		Node* Prev; // 이전 노드의 주소값을 담는 노드 포인터
		Node* Next; // 다음 노드의 주소값을 담는 노드 포인터
	};
	Node mHead;	// 리스트의 헤더(기점)
	Node mTail;	// 리스트의 꼬리(종점)
	int  mSize;	// 노드 개수

public:
	//-------------------------------------------------------------
	// 반복자
	// 설명 : 리스트를 조작하기 위해 반복자를 사용하여
	//		  생성한 리스트 노드를 제어할 수 있다.
	//-------------------------------------------------------------
	class iterator
	{
		friend class CList;

	private:
		Node* mNode;	// 제어 대상 노드의 주소를 저장할 포인터

	public:
		//-------------------------------------------------------------
		// 생성자
		//-------------------------------------------------------------
		iterator(Node* node = nullptr)
		{
			mNode = node;
		}

		//-------------------------------------------------------------
		// 소멸자
		//-------------------------------------------------------------
		~iterator()
		{}

	public:
		Node* GetNode() const
		{
			return mNode;
		}

		//-------------------------------------------------------------
		// 반복자 노드의 데이터를 얻는다.
		//-------------------------------------------------------------
		T& operator *()
		{
			return mNode->Data;
		}

		//-------------------------------------------------------------
		// 반복자 노드의 다음 위치 노드를 얻는다.
		//-------------------------------------------------------------
		iterator& operator ++()
		{
			//-------------------------------------------------------------
			// 1. 자기 자신의 노드 위치를 다음 노드로 갱신
			//-------------------------------------------------------------
			mNode = mNode->Next;

			//-------------------------------------------------------------
			// 2. 자기 자신을 래퍼런스 형식으로 반환
			//-------------------------------------------------------------
			return *this;
		}

		iterator operator ++(int)
		{
			//-------------------------------------------------------------
			// 1. 자기 자신의 현재 노드를 담아둔다.
			//-------------------------------------------------------------
			Node* temp = mNode;

			//-------------------------------------------------------------
			// 2. 자기 자신의 노드 위치를 다음 노드로 갱신
			//-------------------------------------------------------------
			mNode = mNode->Next;

			//-------------------------------------------------------------
			// 3. 갱신 이전의 노드를 반환한다.
			//-------------------------------------------------------------
			return iterator(temp);
		}

		//-------------------------------------------------------------
		// 반복자 노드의 이전 위치 노드를 얻는다.
		//-------------------------------------------------------------
		iterator& operator --()
		{
			//-------------------------------------------------------------
			// 1. 자기 자신의 노드 위치를 이전 노드로 갱신
			//-------------------------------------------------------------
			mNode = mNode->Prev;

			//-------------------------------------------------------------
			// 2. 자기 자신을 래퍼런스 형식으로 반환
			//-------------------------------------------------------------
			return *this;
		}

		iterator operator --(int)
		{
			//-------------------------------------------------------------
			// 1. 자기 자신의 현재 노드를 담아둔다.
			//-------------------------------------------------------------
			Node* temp = mNode;

			//-------------------------------------------------------------
			// 2. 자기 자신의 노드 위치를 이전 노드로 갱신
			//-------------------------------------------------------------
			mNode = mNode->Prev;

			//-------------------------------------------------------------
			// 3. 갱신 이전의 노드를 반환한다.
			//-------------------------------------------------------------
			return iterator(temp);
		}

		//-------------------------------------------------------------
		// 반복자 노드와 인자로 들어온 반복자 노드가 같은지 검사한다.
		//-------------------------------------------------------------
		bool operator ==(const iterator& other)
		{
			if (mNode == other.mNode)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		//-------------------------------------------------------------
		// 반복자 노드와 인자로 들어온 반복자 노드가 다른지 검사한다.
		//-------------------------------------------------------------
		bool operator !=(const iterator& other)
		{
			if (mNode != other.mNode)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	};

public:
	void Swap(Node* node1, Node* node2)
	{
		node1->Prev->Next = node2;
		node2->Next->Prev = node1;

		node1->Next = node2->Next;
		node2->Prev = node1->Prev;

		node1->Prev = node2;
		node2->Next = node1;
	}

	//-------------------------------------------------------------
	// 리스트의 마지막 데이터 뒤에 새로운 데이터를 추가한다.
	//-------------------------------------------------------------
	void push_back(const T data)
	{
		//-------------------------------------------------------------
		// 1. 새로운 노드 생성 후 데이터 넣기
		//-------------------------------------------------------------
		Node* newNode = new Node;
		newNode->Data = data;

		//-------------------------------------------------------------
		// 2. 새로운 노드의 다음과 이전 갱신
		//-------------------------------------------------------------
		newNode->Next = &mTail;
		newNode->Prev = mTail.Prev;

		//-------------------------------------------------------------
		// 3. 꼬리(종점) 노드와 꼬리 노드가 가리키는 노드 갱신
		//-------------------------------------------------------------
		mTail.Prev->Next = newNode;
		mTail.Prev = newNode;

		//-------------------------------------------------------------
		// 4. 노드 개수 증가
		//-------------------------------------------------------------
		mSize++;
	}


	//-------------------------------------------------------------
	// 리스트의 처음 데이터 앞에 새로운 데이터를 추가한다.
	//-------------------------------------------------------------
	void push_front(const T data)
	{
		//-------------------------------------------------------------
		// 1. 새로운 노드 생성 후 데이터 넣기
		//-------------------------------------------------------------
		Node* newNode = new Node;
		newNode->Data = data;

		//-------------------------------------------------------------
		// 2. 새로운 노드의 다음과 이전 갱신
		//-------------------------------------------------------------
		newNode->Prev = &mHead;
		newNode->Next = mHead.Next;

		//-------------------------------------------------------------
		// 3. 헤더 노드와 헤더 노드가 가리키는 노드 갱신
		//-------------------------------------------------------------
		mHead.Next->Prev = newNode;
		mHead.Next = newNode;

		//-------------------------------------------------------------
		// 4. 노드 개수 증가
		//-------------------------------------------------------------
		mSize++;
	}


	//-------------------------------------------------------------
	// 리스트의 마지막 데이터를 뽑아온다.
	//-------------------------------------------------------------
	T pop_back()
	{
		//-------------------------------------------------------------
		// 1. 마지막 노드의 데이터 뽑기
		//-------------------------------------------------------------
		T data = mTail.Prev->Data;

		//-------------------------------------------------------------
		// 2. 마지막 노드 위치 저장
		//-------------------------------------------------------------
		Node* endNode = mTail.Prev;

		//-------------------------------------------------------------
		// 3. 꼬리 노드 이전 노드의 이전 노드의 다음 노드와 꼬리 노드를 연결
		//    꼬리 노드 이전 노드와 꼬리 노드의 이전 노드의 이전 노드를 연결
		//-------------------------------------------------------------
		mTail.Prev->Prev->Next = &mTail;
		mTail.Prev = mTail.Prev->Prev;

		//-------------------------------------------------------------
		// 4. 저장해둔 마지막 노드 삭제
		//-------------------------------------------------------------
		delete endNode;

		//-------------------------------------------------------------
		// 5. 노드 개수 감소
		//-------------------------------------------------------------
		mSize--;

		//-------------------------------------------------------------
		// 6. 데이터 리턴
		//-------------------------------------------------------------
		return data;
	}


	//-------------------------------------------------------------
	// 리스트의 처음 데이터를 뽑아온다.
	//-------------------------------------------------------------
	T pop_front()
	{
		//-------------------------------------------------------------
		// 1. 처음 노드의 데이터 뽑기
		//-------------------------------------------------------------
		T data = mHead.Next->Data;

		//-------------------------------------------------------------
		// 2. 처음 노드 위치 저장
		//-------------------------------------------------------------
		Node* startNode = mHead.Next;

		//-------------------------------------------------------------
		// 3. 헤더 노드 다음 노드의 다음 노드의 이전 노드와 헤더 노드를 연결
		//    헤더 노드 다음 노드와 헤더 노드의 다음 노드의 다음 노드를 연결
		//-------------------------------------------------------------
		mHead.Next->Next->Prev = &mHead;
		mHead.Next = mHead.Next->Next;

		//-------------------------------------------------------------
		// 4. 저장해둔 시작 노드 삭제
		//-------------------------------------------------------------
		delete startNode;

		//-------------------------------------------------------------
		// 5. 노드 개수 감소
		//-------------------------------------------------------------
		mSize--;

		//-------------------------------------------------------------
		// 6. 데이터 리턴
		//-------------------------------------------------------------
		return data;
	}


	//-------------------------------------------------------------
	// 리스트를 초기화한다.
	//-------------------------------------------------------------
	void clear()
	{
		Node* node = mHead.Next;

		//-------------------------------------------------------------
		// [ node ] 가 Tail 노드가 아닐 때 까지 반복
		//-------------------------------------------------------------
		while (node != &mTail)
		{
			//-------------------------------------------------------------
			// 1. 현재 노드를 담아둔다.
			//-------------------------------------------------------------
			Node* temp = node;

			//-------------------------------------------------------------
			// 2. 현재 노드를 다음 노드로 갱신한다.
			//-------------------------------------------------------------
			node = node->Next;

			//-------------------------------------------------------------
			// 3. 담아둔 현재 노드를 삭제한다.
			//-------------------------------------------------------------
			delete temp;
		}
	}


	//-------------------------------------------------------------
	// 리스트의 노드 개수를 반환한다.
	//-------------------------------------------------------------
	const int size() const
	{
		return mSize;
	}


	//-------------------------------------------------------------
	// 리스트가 비어있는지 검사한다.
	//-------------------------------------------------------------
	const bool empty() const
	{
		if (mSize == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//-------------------------------------------------------------
	// 리스트의 헤더(시작점) 다음 노드를 담은 이터레이터 인스턴스를 반환한다.
	//-------------------------------------------------------------
	iterator begin()
	{
		return iterator(mHead.Next);
	}


	//-------------------------------------------------------------
	// 리스트의 꼬리(종점)를 담은 이터레이터 인스턴스를 반환한다.
	//-------------------------------------------------------------
	iterator end()
	{
		return iterator(&mTail);
	}


	//-------------------------------------------------------------
	// 인자로 들어온 이터레이터가 가리키는 노드를 삭제한 후 
	// 삭제한 노드 다음 노드를 담은 이터레이터 인스턴스를 반환한다.
	//-------------------------------------------------------------
	iterator erase(iterator iter)
	{
		//-------------------------------------------------------------
		// 현재 이터레이터가 가리키는 노드가 꼬리 노드가 아닌 경우만 삭제
		//-------------------------------------------------------------
		if (iter.mNode != &mTail)
		{
			//-------------------------------------------------------------
			// 다음 노드를 담아둔다.
			//-------------------------------------------------------------
			Node* temp = iter.mNode->Next;

			//-------------------------------------------------------------
			// 1. 현재 노드의 이전 노드를 현재 노드의 다음 노드와 연결
			//-------------------------------------------------------------
			iter.mNode->Prev->Next = iter.mNode->Next;

			//-------------------------------------------------------------
			// 2. 현재 노드의 다음 노드를 현재 노드의 이전 노드와 연결
			//-------------------------------------------------------------
			iter.mNode->Next->Prev = iter.mNode->Prev;

			//-------------------------------------------------------------
			// 3. 현재 이터레이터의 노드 삭제
			//-------------------------------------------------------------
			delete iter.mNode;

			mSize -= 1;

			//-------------------------------------------------------------
			// 4. 담아둔 다음 노드를 가리키는 이터레이터 반환
			//-------------------------------------------------------------
			return iterator(temp);
		}

		return iterator(nullptr);
	}


	//-------------------------------------------------------------
	// 인자로 들어온 데이터 값과 동일한 노드를 찾아서
	// 해당 노드를 삭제한다.
	//-------------------------------------------------------------
	void remove(T data)
	{
		//-------------------------------------------------------------
		// [ data ] 를 가지고 있는 노드를 찾아서 삭제.
		//-------------------------------------------------------------
		for (CList<T>::iterator iter = begin(); iter != end();)
		{
			//-------------------------------------------------------------
			// 1. 일치하는 데이터가 [있다]
			//-------------------------------------------------------------
			if (*iter == data)
			{
				// 삭제 후 이터레이터 갱신
				iter = erase(iter);
			}
			//-------------------------------------------------------------
			// 2. 일치하는 데이터가 [없다]
			//-------------------------------------------------------------
			else
			{
				// 이터레이터의 노드 위치만 갱신
				++iter;
			}
		}
	}
};