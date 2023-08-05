#pragma once

namespace lesson_16
{
	// - RefCountable과 TSharedPtr의 문제점
	// 1) 다른 사람이 만들어서 배포한 클래스에는 사용하기 어려움 (상속을 해야하기 때문에)
	// 2) 순환 (Cycle) 문제

	class RefCountable
	{
	public:
		RefCountable() : _refCount(1) { }
		virtual ~RefCountable() { }

		int32 GetRefCount() { return _refCount; }

		int32 AddRef() { return ++_refCount; }
		int32 ReleaseRef()
		{
			int32 refCount = --_refCount;
			if (refCount == 0)
			{
				delete this;
			}
			return refCount;
		}

	protected:
		atomic<int32> _refCount;
	};

	template<typename T>
	class TSharedPtr
	{
	public:
		TSharedPtr() { }
		TSharedPtr(T* ptr) { Set(ptr); }

		// 복사
		TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
		// 이동
		TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }
		// 상속 관계 복사
		template<typename U>
		TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

		~TSharedPtr() { Release(); }

	public:
		// 복사 연산자
		TSharedPtr& operator=(const TSharedPtr& rhs)
		{
			if (_ptr != rhs._ptr)
			{
				Release();
				Set(rhs._ptr);
			}
			return *this;
		}

		// 이동 연산자
		TSharedPtr& operator=(TSharedPtr&& rhs)
		{
			Release();
			_ptr = rhs._ptr;
			rhs._ptr = nullptr;
			return *this;
		}

		bool		operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
		bool		operator==(T* ptr) const { return _ptr == ptr; }
		bool		operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
		bool		operator!=(T* ptr) const { return _ptr != ptr; }
		bool		operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
		T*			operator*() { return _ptr; }
		const T*	operator*() const { return _ptr; }
		operator T* () const { return _ptr; }
		T*			operator->() { return _ptr; }
		const T*	operator->() const { return _ptr; }

		bool IsNull() { return _ptr == nullptr; }

	private:
		void Set(T* ptr)
		{
			_ptr = ptr;
			if (ptr)
				ptr->AddRef();
		}

		void Release()
		{
			if (_ptr)
			{
				_ptr->ReleaseRef();
				_ptr = nullptr;
			}
		}

	private:
		T* _ptr = nullptr;
	};

	using WraithRef = TSharedPtr<class Wraith>;
	using MissileRef = TSharedPtr<class Missile>;

	class Wraith : public RefCountable
	{
	public:
		int _hp   = 150;
		int _posX = 0;
		int _posY = 0;
	};

	class Missile : public RefCountable
	{
	public:
		void SetTarget(WraithRef target)
		{
			_target = target;
		}

		bool Update()
		{
			if (_target == nullptr)
				return true;

			if (_target->_hp == 0)
				return true;

			int posX = _target->_posX;
			int posY = _target->_posY;

			return false;
		}

		WraithRef _target = nullptr;
	};

	using KnightRef = TSharedPtr<class Knight>;
	using InventoryRef = TSharedPtr<class Inventory>;

	class Knight : public RefCountable
	{
	public:
		Knight()
		{
			cout << "Knight()" << endl;
		}

		~Knight()
		{
			cout << "~Knight()" << endl;
		}

		void SetTarget(KnightRef target)
		{
			_target = target;
		}

		KnightRef _target = nullptr;
		InventoryRef _inventory = nullptr;
	};

	class Inventory : public RefCountable
	{
	public:
		Inventory(KnightRef knight) : _knight(knight) { }

		//Knight& _knight
		KnightRef _knight;
	};

	void lesson_16()
	{
		// case 1: 스마트 포인터의 메모리 자동 해제
		//while (true)
		{
			WraithRef wraith(new Wraith());
			wraith->ReleaseRef();

			MissileRef missile(new Missile());
			missile->ReleaseRef();

			missile->SetTarget(wraith);

			// 레이스가 피격 당함
			wraith->_hp = 0;

			while (true)
			{
				if (missile)
				{
					if (missile->Update())
						break;
				}
			}
		}

		// case 2: 순환 문제로 인한 메모리 릭 (1)
		//while (true)
		{
			KnightRef k1(new Knight());
			k1->ReleaseRef();

			KnightRef k2(new Knight());
			k2->ReleaseRef();

			k1->SetTarget(k2);
			k2->SetTarget(k1);

			// 순환을 끊는 것이 필요함
			//k1->SetTarget(nullptr);
			//k2->SetTarget(nullptr);
		}

		// case 2: 순환 문제로 인한 메모리 릭 (2)
		//while (true)
		{
			KnightRef k1(new Knight());
			k1->ReleaseRef();

			k1->_inventory = new Inventory(k1);
		}

		// 표준 스마트 포인터 삼총사 (unique_ptr | shared_ptr | weak_ptr)
		{
			//// unique_ptr
			unique_ptr<Knight> k1 = make_unique<Knight>();
			//unique_ptr<Knight> k2 = k1; // 복사가 막혀있음
			unique_ptr<Knight> k3 = move(k1); // 이동은 가능함

			//// shared_ptr
			// [Knight][RefCountBlock(useCount(shared), weakCount)]
			//    |            |
			// [Knight*][RefCountBlock*]
			shared_ptr<Knight> spr(new Knight());

			// [Knight + RefCountBlock(useCount(shared), weakCount)]
			//    |            |
			// [Knight*][RefCountBlock*]
			shared_ptr<Knight> spr2(make_shared<Knight>());

			//// weak_ptr
			weak_ptr<Knight> wpr = spr;

			// 객체가 아직 존재하는지 확인
			bool isExpired = wpr.expired();

			// weak_ptr -> shared_ptr
			shared_ptr<Knight> spr3 = wpr.lock();
			if (spr3 != nullptr)
			{
				// Do Something
			}
		}
	}
}
