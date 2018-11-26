#ifndef ZSCRIPT_TYPES_H
#define ZSCRIPT_TYPES_H

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "CompilerUtils.h"

namespace ZScript
{
	////////////////////////////////////////////////////////////////
	// Forward Declarations
	class Function;
	class Scope;
	class ZClass;
	class DataType;

	typedef int DataTypeId;

	////////////////////////////////////////////////////////////////
	// Stores and lookup types and classes.
	class TypeStore
	{
	public:
		TypeStore();
		~TypeStore();

		// Types
		DataType const* getType(DataTypeId typeId) const;
		optional<DataTypeId> getTypeId(DataType const& type) const;
		optional<DataTypeId> assignTypeId(DataType const& type);
		optional<DataTypeId> getOrAssignTypeId(DataType const& type);

		template <typename Type>
		Type const* getCanonicalType(Type const& type)
		{
			return static_cast<Type const*>(
					ownedTypes[*getOrAssignTypeId(type)]);
		}
	
		// Classes
		std::vector<ZScript::ZClass*> getClasses() const {
			return ownedClasses;}
		ZScript::ZClass* getClass(int classId) const;
		ZScript::ZClass* createClass(std::string const& name);

	private:
		// Comparator for pointers to types.
		struct TypeIdMapComparator {
			bool operator() (
					DataType const* const& lhs, DataType const* const& rhs)
					const;
		};

		std::vector<DataType const*> ownedTypes;
		std::map<DataType const*, DataTypeId, TypeIdMapComparator> typeIdMap;
		std::vector<ZClass*> ownedClasses;
	};

	std::vector<Function*> getClassFunctions(TypeStore const&);

	////////////////////////////////////////////////////////////////
	// Data Types

	enum DataTypeIdBuiltin
	{
		ZVARTYPEID_START = 0,

		ZVARTYPEID_PRIMITIVE_START = 0,
		ZVARTYPEID_UNTYPED = 0,
		ZVARTYPEID_VOID,
		ZVARTYPEID_FLOAT,
		ZVARTYPEID_BOOL,
		ZVARTYPEID_PRIMITIVE_END,

		ZVARTYPEID_CONST_FLOAT = ZVARTYPEID_PRIMITIVE_END,

		ZVARTYPEID_CLASS_START,
		ZVARTYPEID_GAME = ZVARTYPEID_CLASS_START,
		ZVARTYPEID_LINK,
		ZVARTYPEID_SCREEN,
		ZVARTYPEID_FFC,
		ZVARTYPEID_ITEM,
		ZVARTYPEID_ITEMCLASS,
		ZVARTYPEID_NPC,
		ZVARTYPEID_LWPN,
		ZVARTYPEID_EWPN,
		ZVARTYPEID_NPCDATA,
		ZVARTYPEID_DEBUG,
		ZVARTYPEID_AUDIO,
		ZVARTYPEID_COMBOS,
		ZVARTYPEID_SPRITEDATA,
		ZVARTYPEID_GRAPHICS,
		ZVARTYPEID_BITMAP,
		ZVARTYPEID_TEXT,
		ZVARTYPEID_INPUT,
		ZVARTYPEID_MAPDATA,
		ZVARTYPEID_DMAPDATA,
		ZVARTYPEID_ZMESSAGE,
		ZVARTYPEID_SHOPDATA,
		ZVARTYPEID_DROPSET,
		ZVARTYPEID_PONDS,
		ZVARTYPEID_WARPRING,
		ZVARTYPEID_DOORSET,
		ZVARTYPEID_ZUICOLOURS,
		ZVARTYPEID_RGBDATA,
		ZVARTYPEID_PALETTE,
		ZVARTYPEID_TUNES,
		ZVARTYPEID_PALCYCLE,
		ZVARTYPEID_GAMEDATA,
		ZVARTYPEID_CHEATS,
		ZVARTYPEID_CLASS_END,

		ZVARTYPEID_END = ZVARTYPEID_CLASS_END
	};

	class DataTypeSimple;
	class DataTypeConstFloat;
	class DataTypeClass;
	class DataTypeArray;

	class DataType
	{
	public:
		// Call derived class's copy constructor.
		virtual DataType* clone() const = 0;

		// Resolution.
		virtual bool isResolved() const {return true;}
		virtual DataType* resolve(ZScript::Scope& scope) {return this;}

		// Basics
		virtual std::string getName() const = 0;
		virtual bool canCastTo(DataType const& target) const = 0;
		virtual bool canBeGlobal() const {return false;}

		// Derived class info.
		virtual bool isArray() const {return false;}
		virtual bool isClass() const {return false;}

		// Returns <0 if <rhs, 0, if ==rhs, and >0 if >rhs.
		int compare(DataType const& rhs) const;
	
	private:
		// Returns <0 if <rhs, 0, if ==rhs, and >0 if >rhs.
		// rhs is guaranteed to be the same class as the derived type.
		virtual int selfCompare(DataType const& rhs) const = 0;

		// Standard Types.
	public:
		static DataTypeSimple const UNTYPED;
		static DataTypeSimple const ZVOID;
		static DataTypeSimple const FLOAT;
		static DataTypeSimple const BOOL;
		static DataTypeConstFloat const CONST_FLOAT;
		static DataTypeArray const STRING;
		static DataTypeClass const FFC;
		static DataTypeClass const ITEM;
		static DataTypeClass const ITEMCLASS;
		static DataTypeClass const NPC;
		static DataTypeClass const LWPN;
		static DataTypeClass const EWPN;
		static DataTypeClass const GAME;
		static DataTypeClass const LINK;
		static DataTypeClass const SCREEN;
		static DataTypeClass const AUDIO;
		static DataTypeClass const DEBUG;
		static DataTypeClass const NPCDATA;
		static DataTypeClass const COMBOS;
		static DataTypeClass const SPRITEDATA;
		static DataTypeClass const GRAPHICS;
		static DataTypeClass const BITMAP;
		static DataTypeClass const TEXT;
		static DataTypeClass const INPUT;
		static DataTypeClass const MAPDATA;
		static DataTypeClass const DMAPDATA;
		static DataTypeClass const ZMESSAGE;
		static DataTypeClass const SHOPDATA;
		static DataTypeClass const DROPSET;
		static DataTypeClass const PONDS;
		static DataTypeClass const WARPRING;
		static DataTypeClass const DOORSET;
		static DataTypeClass const ZUICOLOURS;
		static DataTypeClass const RGBDATA;
		static DataTypeClass const PALETTE;
		static DataTypeClass const TUNES;
		static DataTypeClass const PALCYCLE;
		static DataTypeClass const GAMEDATA;
		static DataTypeClass const CHEATS;
		static DataType const* get(DataTypeId id);
	};

	bool operator==(DataType const&, DataType const&);
	bool operator!=(DataType const&, DataType const&);
	bool operator<(DataType const&, DataType const&);
	bool operator<=(DataType const&, DataType const&);
	bool operator>(DataType const&, DataType const&);
	bool operator>=(DataType const&, DataType const&);

	// Get the data type stripped of consts and arrays.
	DataType const& getNaiveType(DataType const& type);
	
	// Get the number of nested arrays at top level.
	int getArrayDepth(DataType const&);

	class DataTypeUnresolved : public DataType
	{
	public:
		DataTypeUnresolved(std::string const& name) : name(name) {}
		DataTypeUnresolved* clone() const {
			return new DataTypeUnresolved(*this);}
		
		virtual bool isResolved() const {return false;}
		virtual DataType* resolve(ZScript::Scope& scope);

		virtual std::string getName() const {return name;}
		virtual bool canCastTo(DataType const& target) const {return false;}

	private:
		std::string name;

		int selfCompare(DataType const& rhs) const;
	};

	class DataTypeSimple : public DataType
	{
	public:
		DataTypeSimple(int simpleId, std::string const& name);
		DataTypeSimple* clone() const {return new DataTypeSimple(*this);}

		virtual DataTypeSimple* resolve(ZScript::Scope&) {return this;}
		
		virtual std::string getName() const {return name;}
		virtual bool canCastTo(DataType const& target) const;
		virtual bool canBeGlobal() const;

		int getId() const {return simpleId;}

	private:
		int simpleId;
		std::string name;

		int selfCompare(DataType const& rhs) const;
	};

	// Temporary while only floats can be constant.
	class DataTypeConstFloat : public DataType
	{
	public:
		DataTypeConstFloat() {}
		DataType* clone() const {return new DataTypeConstFloat(*this);}
		
		virtual DataTypeConstFloat* resolve(ZScript::Scope& scope) {
			return this;}

		virtual std::string getName() const {return "const float";}
		virtual bool canCastTo(DataType const& target) const;
		virtual bool canBeGlobal() const {return true;}

	private:
		int selfCompare(DataType const& other) const {return 0;};
	};

	class DataTypeClass : public DataType
	{
	public:
		DataTypeClass(int classId);
		DataTypeClass(int classId, std::string const& className);
		DataTypeClass* clone() const {return new DataTypeClass(*this);}

		virtual DataTypeClass* resolve(ZScript::Scope& scope);

		virtual std::string getName() const;
		virtual bool canCastTo(DataType const& target) const;
		virtual bool canBeGlobal() const {return true;}
		virtual bool isClass() const {return true;}

		std::string getClassName() const {return className;}
		int getClassId() const {return classId;}
		
	private:
		int classId;
		std::string className;

		int selfCompare(DataType const& other) const;
	};

	class DataTypeArray : public DataType
	{
	public:
		DataTypeArray(DataType const& elementType)
			: elementType(elementType) {}
		DataTypeArray* clone() const {return new DataTypeArray(*this);}

		virtual DataTypeArray* resolve(ZScript::Scope& scope) {return this;}

		virtual std::string getName() const {
			return elementType.getName() + "[]";}
		virtual bool canCastTo(DataType const& target) const;
		virtual bool canBeGlobal() const {return true;}
		virtual bool isArray() const {return true;}

		DataType const& getElementType() const {return elementType;}

	private:
		DataType const& elementType;

		int selfCompare(DataType const& other) const;
	};

	DataType const& getBaseType(DataType const&);

	////////////////////////////////////////////////////////////////
	// Script Types

	// Basically an enum.
	class ScriptType
	{
		friend bool operator==(ScriptType const& lhs, ScriptType const& rhs);
		friend bool operator!=(ScriptType const& lhs, ScriptType const& rhs);

	public:
		enum Id
		{
			idInvalid,
			idStart,
			idGlobal = idStart,
			idFfc,
			idItem,
			idNPC,
			idEWeapon,
			idLWeapon,
			idLink,
			idScreen,
			idDMap,
			
			idEnd
		};
	
		ScriptType() : id_(idInvalid) {}
		
		std::string const& getName() const;
		DataTypeId getThisTypeId() const;
		bool isValid() const {return id_ >= idStart && id_ < idEnd;}

		static ScriptType const invalid;
		static ScriptType const global;
		static ScriptType const ffc;
		static ScriptType const item;
		static ScriptType const npc;
		static ScriptType const eweapon;
		static ScriptType const lweapon;
		static ScriptType const link;
		static ScriptType const dmap;
		static ScriptType const screen;

	private:
		ScriptType(Id id) : id_(id) {}
		
		Id id_;
	};

	// All invalid values are equal to each other.
	bool operator==(ScriptType const& lhs, ScriptType const& rhs);
	bool operator!=(ScriptType const& lhs, ScriptType const& rhs);
}

#endif
