
#pragma once
/* file: "/mnt/c/cloud/local/dev/langs/escript/../EScript/EScript.h" */
/*
**  Copyright (c) Claudius Jaehn (claudiusj@users.berlios.de), 2007-2013
**
**  Permission is hereby granted, free of charge, to any person obtaining a copy of this
**  software and associated documentation files (the "Software"), to deal in the Software
**  without restriction, including without limitation the rights to use, copy, modify,
**  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
**  permit persons to whom the Software is furnished to do so, subject to the following
**  conditions:
**
**  The above copyright notice and this permission notice shall be included in all copies
**  or substantial portions of the Software.
**
**  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
**  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
**  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
**  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
**  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
**  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <utility>
#include <cstddef>
#include <type_traits>
#include <cstdint>
#include <unordered_map>
#include <ostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <memory>
#include <iosfwd>
#include <limits>
#include <cmath>
#include <deque>
#include <set>

#if defined(__GNUC__)
    #define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define DEPRECATED __declspec(deprecated)
#else
    #pragma message("WARNING: DEPRECATED not set for this compiler")
    #define DEPRECATED
#endif


#define ES_FUNCTION2 ES_FUNCTION

#define ES_FUNCTION_DEF_OLD_(_name)                                                                                    \
    static EScript::RtValue _name(EScript::Runtime& runtime UNUSED_ATTRIBUTE, EScript::ObjPtr caller UNUSED_ATTRIBUTE, \
                                  const EScript::ParameterValues& parameter UNUSED_ATTRIBUTE)
                                  
// macros which can be used inside init(...) functions

/*! Macro for defining and declaring a (simple) EScript function in short form.
	\example
		// Within the Collection::init(...) function:
		ESF_DECLARE(typeObj, "==", 1, 1, assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0]))
*/
#define ESF_DECLARE(_obj, _fnNameStr, _min, _max, _returnExpr)               \
    {                                                                        \
        struct _fnWrapper                                                    \
        {                                                                    \
            ES_FUNCTION_DEF_OLD_(function)                                   \
            {                                                                \
                return EScript::value(_returnExpr);                          \
            }                                                                \
        };                                                                   \
        declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
    }

/*! Macro for defining and declaring a (simple) EScript member function in short form.
	\note The variable \a self contains the caller with the type \a _objType (otherwise an exception is thrown).
	\example
		// Within the Collection::init(...) function:
		ESMF_DECLARE(typeObj, Collection, "==", 1, 1, self->rt_isEqual(runtime, parameter[0]))
*/
#define ESMF_DECLARE(_obj, _objType, _fnNameStr, _min, _max, _returnExpr)    \
    {                                                                        \
        struct _fnWrapper                                                    \
        {                                                                    \
            ES_FUNCTION_DEF_OLD_(function)                                   \
            {                                                                \
                EScript::ObjPtr thisEObj(caller);                            \
                _objType* self = thisEObj.to<_objType*>(runtime);            \
                return EScript::value(_returnExpr);                          \
            }                                                                \
        };                                                                   \
        declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
    }

/*! Macro for defining and declaring an EScript function.
	\example
		// Within the Collection::init(...) function:
		ES_FUNCTION_DECLARE(typeObj, "==", 1, 1, {
				return assertType<Collection>(runtime, caller)->rt_isEqual(runtime, parameter[0]);
		})
*/
#define ES_FUNCTION_DECLARE(_obj, _fnNameStr, _min, _max, _block)            \
    {                                                                        \
        struct _fnWrapper                                                    \
        {                                                                    \
            ES_FUNCTION_DEF_OLD_(function)                                   \
            {                                                                \
                do                                                           \
                    _block while(false);                                     \
            }                                                                \
        };                                                                   \
        declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
    }

/*! Macro for defining and declaring an EScript member function.
	\example
		// Within the Collection::init(...) function:
		ES_MFUNCTION_DECLARE(typeObj, Collection, "==", 1, 1, {
				return self->rt_isEqual(runtime, parameter[0]);
		})
*/
#define ES_MFUNCTION_DECLARE(_obj, _objType, _fnNameStr, _min, _max, _block) \
    {                                                                        \
        struct _fnWrapper                                                    \
        {                                                                    \
            ES_FUNCTION_DEF_OLD_(function)                                   \
            {                                                                \
                EScript::ObjPtr thisEObj(caller);                            \
                _objType* self = thisEObj.to<_objType*>(runtime);            \
                do                                                           \
                    _block while(false);                                     \
            }                                                                \
        };                                                                   \
        declareFunction(_obj, _fnNameStr, _min, _max, _fnWrapper::function); \
    }

/*! Macro for defining a (simple) EScript function in short form.
	\example
		ESF(esmf_Collection_equal, 1, 1, assertType<Collection>(runtime,caller)->rt_isEqual(runtime, parameter[0]))
	\deprecated
*/
#define ESF(_fnName, _min, _max, _returnExpr)                              \
    ES_FUNCTION_DEF_OLD_(_fnName)                                          \
    {                                                                      \
        EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
        return (_returnExpr);                                              \
    }

/*! Macro for defining a (simple) EScript member function in short form.
	\note The variable \a self contains the caller with the type \a _objType (otherwise an exception is thrown).
	\example
		ESMF(Collection, esmf_Collection_equal, 1, 1, self->rt_isEqual(runtime, parameter[0]))
	\deprecated
*/
#define ESMF(_objType, _fnName, _min, _max, _returnExpr)                   \
    ES_FUNCTION_DEF_OLD_(_fnName)                                          \
    {                                                                      \
        EScript::assertParamCount(runtime, parameter.count(), _min, _max); \
        EScript::ObjPtr thisEObj(caller);                                  \
        _objType* self = thisEObj.to<_objType*>(runtime);                  \
        return (_returnExpr);                                              \
    }

/*! Defines conversion functions for converting an EScript object to a contained C++ object.
	ES_CONV_EOBJ_TO_OBJ( type of EScript object, target type, expression to convert the eObj to the target type)
	- Internally specifies two template functions; one for "target_t" and one for "const target_t".
	- Needs to be located inside the global namespace.
	- The EScript object of type eSource_t is available in the expression by the variable "eObj".
	- If a specialized implementation is needed that does not fit this macro, manually provide a
	  template specialization of the convertTo<>(..) method.
	\code
	  ES_CONV_EOBJ_TO_OBJ(E_SomeReferenceObj, SomeWrappedObject&, **eObj) // simple example
	\endcode
*/
#define ES_CONV_EOBJ_TO_OBJ(eSource_t, target_t, expression)                                     \
    namespace EScript                                                                            \
    {                                                                                            \
        template <> inline target_t convertTo<target_t>(Runtime & rt, ObjPtr source)             \
        {                                                                                        \
            eSource_t* eObj = assertType<eSource_t>(rt, source);                                 \
            return (expression);                                                                 \
        }                                                                                        \
        template <> inline const target_t convertTo<const target_t>(Runtime & rt, ObjPtr source) \
        {                                                                                        \
            eSource_t* eObj = assertType<eSource_t>(rt, source);                                 \
            return (expression);                                                                 \
        }                                                                                        \
    }
/*! Defines a factory function for creating an EScript object for a given C++ object.
	ES_CONV_OBJ_TO_EOBJ(source type, target type of EScript object, expression to create an EScript object)
	- Needs to be located inside the global namespace.
	- The C++ object of type source_t is available in the expression by the variable "obj".
	- If a specialized implementation is needed that does not fit this macro, manually overload
	  the method eTarget * create(source_t);
	\code
	  ES_CONV_OBJ_TO_EOBJ(const SomeWrappedObject&,E_SomeReferenceObj, new E_SomeReferenceObj(obj))
	  ES_CONV_OBJ_TO_EOBJ(SomeWrappedObject*, E_SomeReferenceObj, obj ? new E_SomeReferenceObj(*obj) : nullptr)
	\endcode
*/
#define ES_CONV_OBJ_TO_EOBJ(source_t, eTarget_t, expression) \
    namespace EScript                                        \
    {                                                        \
        inline eTarget_t* create(source_t obj)               \
        {                                                    \
            return (expression);                             \
        }                                                    \
    }


    #if !defined(_MSC_VER)
        #define UNUSED_ATTRIBUTE __attribute__((unused))
    #else
        #define UNUSED_ATTRIBUTE
    #endif

    #define ES_FUNCTION_DEF_(_name)                                                                                     \
        static EScript::RtValue _name(EScript::Runtime& rt UNUSED_ATTRIBUTE, EScript::ObjPtr thisEObj UNUSED_ATTRIBUTE, \
                                      const EScript::ParameterValues& parameter UNUSED_ATTRIBUTE)
    // macros which can be used inside init(...) functions
    // general functions

    /*! Macro for defining and declaring an EScript function.
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
	\example
		// Within the SomeLib::init(...) function:
		ES_FUNCTION(typeObj, "someStaticFunction", 1, 1, {
				return someStaticFunction(parameter[0].to<const SomeValue&>(rt));
		})
*/
    #define ES_FUNCTION(_typeObject, _fnNameStr, _min, _max, _block)                    \
        {                                                                               \
            struct _fnWrapper                                                           \
            {                                                                           \
                ES_FUNCTION_DEF_(function) _block                                       \
            };                                                                          \
            declareFunction(_typeObject, _fnNameStr, _min, _max, _fnWrapper::function); \
        }

    /*! Macro for defining and declaring a (simple) EScript function in short form.
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
	\example
		// Within the SomeLib::init(...) function:
		ES_FUN(typeObj, "someStaticFunction", 1, 1, someStaticFunction(parameter[0].to<const SomeValue&>(rt)))
*/
    #define ES_FUN(_typeObject, _fnNameStr, _min, _max, _returnExpr) \
        ES_FUNCTION(_typeObject, _fnNameStr, _min, _max, { return EScript::value(_returnExpr); })

    /*! Macro for defining and declaring an EScript member function.
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Object thisEObj
		- _targetType thisObj
	\example
		// Within the SomeType::init(...) function:
		ES_MFUNCTION(typeObj, SomeType, "someMethod", 1, 1, {
				return thisObj->someMethod(parameter[0].to<const SomeValue&>(rt));
		})
*/
    #define ES_MFUNCTION(_typeObject, _targetType, _fnNameStr, _min, _max, _block) \
        ES_FUNCTION(_typeObject, _fnNameStr, _min, _max,                           \
                    {                                                              \
                        _targetType* thisObj = thisEObj.to<_targetType*>(rt);      \
                        do                                                         \
                            _block while(false);                                   \
                    })

    /*! Macro for defining and declaring a (simple) EScript member function in short form.
	\note The variable \a thisObj contains the thisEObj with the type \a _targetType (otherwise an exception is thrown).
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Object thisEObj
		- _targetType thisObj
	\example
		// Within the SomeType::init(...) function:
		ES_MFUN(typeObj, SomeType, "someMethod", 1, 1, thisObj->someMethod(parameter[0].to<const SomeValue&>(rt)))
*/
    #define ES_MFUN(_typeObject, _targetType, _fnNameStr, _min, _max, _returnExpr) \
        ES_MFUNCTION(_typeObject, _targetType, _fnNameStr, _min, _max, { return EScript::value(_returnExpr); })

    /*! Macro for defining and declaring a EScript constructor function .
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Type thisType
*/
    #define ES_CONSTRUCTOR(_typeObject, _min, _max, _block)                  \
        ES_MFUNCTION(_typeObject, EScript::Type, "_constructor", _min, _max, \
                     {                                                       \
                         EScript::Type* thisType UNUSED_ATTRIBUTE = thisObj; \
                         do                                                  \
                             _block while(false);                            \
                     })

    /*! Macro for defining and declaring a EScript constructor function in short form .
	Available variables:
		- EScript::Runtime rt
		- EScript::ParameterValues parameter
		- EScript::Type thisType
*/
    #define ES_CTOR(_typeObject, _min, _max, _returnExpr) \
        ES_CONSTRUCTOR(_typeObject, _min, _max, { return EScript::value(_returnExpr); })


/* file: "/mnt/c/cloud/local/dev/langs/escript/escript.h" */

#define ES_PROVIDES_TYPE_NAME(_TYPE_NAME)   \
public:                                     \
    static const char* getClassName()       \
    {                                       \
        return #_TYPE_NAME;                 \
    }                                       \
    virtual const char* getTypeName() const \
    {                                       \
        return getClassName();              \
    }                                       \
                                            \
private:

// huge(always 0) * 10000 + major * 100 + minor
#define ES_VERSION 702
#define ES_VERSION_STRING "EScript 0.7.2 Egon (https://github.com/EScript)\0"
#define ES_VERSION_NAME "Egon\0"

// used for the windows resources file:
#define ES_VERSION_RES 0, 7, 2, 0
#define ES_VERSION_RES_STRING "0,7,2,0\0"

namespace EScript
{
    class Object;
    class Runtime;
    class StringData;
    class Namespace;
    class Function;
    class FnCompileContext;
    class Logger;
    class UserFunction;
    class StaticData;
    class InstructionBlock;
    class Iterator;
    class Array;
    class Bool;
    class Number;
    class String;
    class Void;
    class CodeFragment;
    class ExtObject;
    class RtValue;
    class Type;
    class FunctionCallContext;
    class Exception;
    class YieldIterator;
    class RuntimeInternals;
    class ObjectReleaseHandler;
    class Compiler;
    class StringId;

    template<class _T> class ERef;
    template<class _T> class EPtr;
    template<class _T> class _Ptr;
    template<class _T> class ERef;
    template<typename _T> class _ObjArray;

    namespace AST
    {
        class ASTNode;
        class Block;
    }

    enum variableType_t
    {
        LOCAL_VAR,
        STATIC_VAR
    };

    using ObjPtr = EPtr<Object>;
    using libInitFunction = void(*)(Namespace*);
    using ObjRef = ERef<Object>;
    using ParameterValues = _ObjArray<ObjRef>;
    using internalTypeId_t = uint8_t;
    using hashvalue = unsigned int;

    typedef hashvalue identifierId;
    typedef EPtr<Object> ObjPtr;
    typedef EPtr<const Object> ObjPtr_const;
    typedef ERef<Object> ObjRef;
    typedef _ObjArray<ObjRef> ParameterValues;
    typedef RtValue (*_functionPtr)(Runtime& runtime, ObjPtr caller, const ParameterValues& parameter);
    typedef std::unordered_map<StringId, variableType_t> declaredVariableMap_t;
    typedef std::pair<variableType_t, int> varLocation_t;


    /*! Convert the given EScript object to a desired type -- if the conversion
	fails, an exception is thrown.	*/
    template<typename target_t>
    target_t convertTo(Runtime&, ObjPtr);

    template<class T>
    static T* assertType(Runtime&, const ObjPtr&);

    /* _Internals::doConvertTo(...) works as a dispatcher for ObjPtr.to<target_t>.
	If target_t is a pointer to an EScript object, assertType is called; convertTo<target_t> otherwise. */
    namespace _Internals
    {
        template <typename target_t>
        inline target_t doConvertTo(Runtime& runtime,
                                    const ObjPtr& src,
                                    typename std::enable_if<!std::is_convertible<target_t, const Object*>::value>::type* = nullptr)
        {
            return EScript::convertTo<target_t>(runtime, src);
        }

        template <typename target_t>
        inline target_t doConvertTo(Runtime& runtime,
                                    const ObjPtr& src,
                                    typename std::enable_if<std::is_convertible<target_t, const Object*>::value>::type* = nullptr)
        {
            return EScript::assertType<typename std::remove_pointer<target_t>::type>(runtime, src);
        }
    }

    //! Simple (counted) reference to use with EReferenceCounter
    template <class _T>
    class _CountedRef
    {
        private:
            _T* obj;

        public:
            _CountedRef() : obj(nullptr)
            {
            }
            _CountedRef(_T* _obj) : obj(_obj)
            {
                _T::addReference(obj);
            }
            _CountedRef(const _CountedRef& other) : obj(other.get())
            {
                _T::addReference(obj);
            }
            _CountedRef(_CountedRef&& other) : obj(other.get())
            {
                other.obj = nullptr;
            }
            _CountedRef(const _Ptr<_T>& other) : obj(other.get())
            {
                _T::addReference(obj);
            }

            ~_CountedRef()
            {
                _T::removeReference(obj);
            }

            //! Just set; dont touch the counter.
            void _set(_T* _obj)
            {
                obj = _obj;
            }

            _CountedRef& operator=(const _CountedRef& other)
            {
                if(other.obj != obj)
                {
                    _T::addReference(other.obj);
                    _T::removeReference(obj);
                    _set(other.obj);
                }
                return *this;
            }
            _CountedRef& operator=(_T* _obj)
            {
                if(_obj != obj)
                {
                    _T::addReference(_obj);
                    _T::removeReference(obj);
                    _set(_obj);
                }
                return *this;
            }

            _CountedRef& operator=(_CountedRef&& other)
            {
                swap(other);
                return *this;
            }

            /*! Detach the object from the Reference without decreasing
                the reference counter or deleting the object; sets the Reference
                to nullptr. */
            _T* detach()
            {
                _T* const o = obj;
                _set(nullptr);
                return o;
            }

            /*! Detach the object from the Reference with decreasing
                the reference counter and set the Reference to nullptr.
                The object is not deletet if the counter reaches 0. */
            _T* detachAndDecrease()
            {
                _T* const o = obj;
                _T::decreaseReference(o);
                _set(nullptr);
                return o;
            }

            void swap(_CountedRef& other)
            {
                _T* const tmp = obj;
                obj = other.obj;
                other.obj = tmp;
            }

            //! Returns a pointer to the referenced Object.
            inline _T* get() const
            {
                return obj;
            }

            //! Direct access to the referenced Object.
            inline _T* operator->() const
            {
                return obj;
            }

            //! Returns true if the referenced object is nullptr.
            inline bool isNull() const
            {
                return obj == nullptr;
            }

            //! Returns true if the referenced object is not nullptr.
            inline bool isNotNull() const
            {
                return obj != nullptr;
            }

            inline explicit operator bool() const
            {
                return obj;
            }

            inline bool operator==(const _CountedRef& other) const
            {
                return obj == other.obj;
            }
            inline bool operator==(const _Ptr<_T>& other) const
            {
                return obj == other.get();
            }
            inline bool operator==(const _T* o2) const
            {
                return obj == o2;
            }
            inline bool operator!=(const _CountedRef& other) const
            {
                return obj != other.obj;
            }
            inline bool operator!=(const _Ptr<_T>& other) const
            {
                return obj != other.get();
            }
            inline bool operator!=(const _T* o2) const
            {
                return obj != o2;
            }
    };

    //! Simple wrapper for a pointer to a possibly
    template <class _T>
    class _Ptr
    {
        private:
            _T* obj;

        public:
            _Ptr() : obj(nullptr)
            {
            }
            _Ptr(_T* _obj) : obj(_obj)
            {
            }
            _Ptr(const _CountedRef<_T>& other) : obj(other.get())
            {
            }

            //! Swap the referenced pointers without touching the reference counter.
            void swap(_Ptr& other)
            {
                _T* const tmp = obj;
                obj = other.obj;
                other.obj = tmp;
            }

            //! Tries to convert object to given Type; returns nullptr if object is nullptr or not of given type.
            template <class _T2> _T2* toType() const
            {
                return isNull() ? nullptr : dynamic_cast<_T2*>(obj);
            }
            template <class _T2> _T2* castTo() const
            {
                return dynamic_cast<_T2*>(this->get());
            }

            //! Returns a pointer to the referenced Object.
            inline _T* get() const
            {
                return obj;
            }

            //! Direct access to the referenced Object.
            inline _T* operator->() const
            {
                return obj;
            }

            //! Returns true if the referenced object is nullptr.
            inline bool isNull() const
            {
                return obj == nullptr;
            }

            //! Returns true if the referenced object is not nullptr.
            inline bool isNotNull() const
            {
                return obj != nullptr;
            }

            inline explicit operator bool() const
            {
                return obj;
            }

            inline bool operator==(const _Ptr& other) const
            {
                return obj == other.obj;
            }
            inline bool operator==(const _CountedRef<_T>& other) const
            {
                return obj == other.get();
            }
            inline bool operator==(const _T* o2) const
            {
                return obj == o2;
            }
            inline bool operator!=(const _Ptr& other) const
            {
                return obj != other.obj;
            }
            inline bool operator!=(const _CountedRef<_T>& other) const
            {
                return obj != other.get();
            }
            inline bool operator!=(const _T* o2) const
            {
                return obj != o2;
            }
    };


    /*! Smart pointer for referencing Objects (with implicit handling of the reference counter)
	with some safe conversion functions (toString etc.).
	ERef ---|> _CountedRef	*/
    template <class _T>
    class ERef : public _CountedRef<_T>
    {
        public:
            ERef() : _CountedRef<_T>(nullptr)
            {
            }
            ERef(_T* _obj) : _CountedRef<_T>(_obj)
            {
            }
            ERef(const EPtr<_T>& other) : _CountedRef<_T>(other.get())
            {
            }

            //! Returns a value of the type target_t if possible or throws an exception.
            template <typename target_t> target_t to(Runtime& runtime)
            {
                return _Internals::doConvertTo<target_t>(runtime, *this);
            }

            /*! If the reference is null, the default value is returend;
                otherwise a value of the type target_t is returned if possible or an exception is thrown.*/
            template <typename target_t> target_t to(Runtime& runtime, const target_t& defaultValue)
            {
                return this->isNull() ? defaultValue : _Internals::doConvertTo<target_t>(runtime, *this);
            }

            //! Returns object->toBool() if the referenced object is not nullptr, false otherwise.
            bool toBool() const
            {
                return this->isNull() ? false : this->get()->toBool();
            }

            //! Returns object->toBool() if the referenced object is not nullptr, defaultValue otherwise.
            bool toBool(bool defaultValue) const
            {
                return this->isNull() ? defaultValue : this->get()->toBool();
            }

            //! Returns object->toDbgString() if the referenced object is not nullptr, "[nullptr]" otherwise.
            std::string toDbgString() const
            {
                return this->isNull() ? "[nullptr]" : this->get()->toDbgString();
            }

            //! Returns object->toDouble() if the referenced object is not nullptr, 0.0 otherwise.
            double toDouble() const
            {
                return this->isNull() ? 0.0 : this->get()->toDouble();
            }

            //! Returns object->toDouble() if the referenced object is not nullptr, defaultValue otherwise.
            double toDouble(double defaultValue) const
            {
                return this->isNull() ? defaultValue : this->get()->toDouble();
            }

            //! Returns object->toFloat() if the referenced object is not nullptr, 0.0f otherwise.
            float toFloat() const
            {
                return this->isNull() ? 0.0f : this->get()->toFloat();
            }

            //! Returns object->toFloat() if the referenced object is not nullptr, defaultValue otherwise.
            float toFloat(float defaultValue) const
            {
                return this->isNull() ? defaultValue : this->get()->toFloat();
            }

            //! Returns object->toInt() if the referenced object is not nullptr, 0 otherwise.
            int toInt() const
            {
                return this->isNull() ? 0 : this->get()->toInt();
            }

            //! Returns object->toInt() if the referenced object is not nullptr, defaultValue otherwise.
            int toInt(int defaultValue) const
            {
                return this->isNull() ? defaultValue : this->get()->toInt();
            }

            //! Return object->toUInt() if the referenced object is not nullptr, 0 otherwise.
            unsigned int toUInt() const
            {
                return this->isNull() ? 0u : this->get()->toUInt();
            }

            //! Return object->toUInt() if the referenced object is not nullptr, defaultValue otherwise.
            unsigned int toUInt(unsigned int defaultValue) const
            {
                return this->isNull() ? defaultValue : this->get()->toUInt();
            }

            //! Returns object->toString() if the referenced object is not nullptr, "" otherwise.
            std::string toString() const
            {
                return this->isNull() ? "" : this->get()->toString();
            }

            //! Returns object->toString() if the referenced object is not nullptr, defaultValue otherwise.
            std::string toString(const std::string& defaultValue) const
            {
                return this->isNull() ? defaultValue : this->get()->toString();
            }

            //! Tries to convert object to given Type; returns nullptr if object is nullptr or not of given type.
            template <class _T2> _T2* toType() const
            {
                return this->isNull() ? nullptr : dynamic_cast<_T2*>(this->get());
            }
            template <class _T2> _T2* castTo() const
            {
                return dynamic_cast<_T2*>(this->get());
            }
    };

    /*! Weak smart pointer for referencing Objects (without implicit handling of the reference counter!)
	with some safe conversion functions (toString etc.).	*/
    template <class _T>
    class EPtr
    {
        private:
            _T* obj;

        public:
            EPtr() : obj(nullptr)
            {
            }
            EPtr(_T* _obj) : obj(_obj)
            {
            }
            EPtr(const ERef<_T>& other) : obj(other.get())
            {
            }

            //! Returns a pointer to the referenced Object.
            inline _T* get() const
            {
                return obj;
            }

            //! Direct access to the referenced Object.
            inline _T* operator->() const
            {
                return obj;
            }

            //! Returns true if the referenced object is nullptr.
            inline bool isNull() const
            {
                return obj == nullptr;
            }

            //! Returns true if the referenced object is not nullptr.
            inline bool isNotNull() const
            {
                return obj != nullptr;
            }

            inline explicit operator bool() const
            {
                return obj;
            }

            inline bool operator==(const EPtr& other) const
            {
                return obj == other.obj;
            }
            inline bool operator==(const _T* o2) const
            {
                return obj == o2;
            }
            inline bool operator!=(const EPtr& other) const
            {
                return obj != other.obj;
            }
            inline bool operator!=(const _T* o2) const
            {
                return obj != o2;
            }

            //! Returns a value of the type target_t if possible or throws an exception.
            template <typename target_t> target_t to(Runtime& runtime)
            {
                return _Internals::doConvertTo<target_t>(runtime, *this);
            }

            /*! If the reference is null, the default value is returend;
                otherwise a value of the type target_t is returned if possible or an exception is thrown.*/
            template <typename target_t> target_t to(Runtime& runtime, const target_t& defaultValue)
            {
                return isNull() ? defaultValue : _Internals::doConvertTo<target_t>(runtime, *this);
            }

            //! Returns object->toBool() if the referenced object is not nullptr, false otherwise.
            bool toBool() const
            {
                return isNull() ? false : obj->toBool();
            }

            //! Returns object->toBool() if the referenced object is not nullptr, defaultValue otherwise.
            bool toBool(bool defaultValue) const
            {
                return isNull() ? defaultValue : obj->toBool();
            }

            //! Returns object->toDbgString() if the referenced object is not nullptr, "[nullptr]" otherwise.
            std::string toDbgString() const
            {
                return this->isNull() ? "[nullptr]" : this->get()->toDbgString();
            }

            //! Returns object->toDouble() if the referenced object is not nullptr, 0.0 otherwise.
            double toDouble() const
            {
                return isNull() ? 0.0 : obj->toDouble();
            }

            //! Returns object->toDouble() if the referenced object is not nullptr, defaultValue otherwise.
            double toDouble(double defaultValue) const
            {
                return isNull() ? defaultValue : obj->toDouble();
            }

            //! Returns object->toFloat() if the referenced object is not nullptr, 0.0f otherwise.
            float toFloat() const
            {
                return isNull() ? 0.0f : obj->toFloat();
            }

            //! Returns object->toFloat() if the referenced object is not nullptr, defaultValue otherwise.
            float toFloat(float defaultValue) const
            {
                return isNull() ? defaultValue : obj->toFloat();
            }

            //! Returns object->toInt() if the referenced object is not nullptr, 0 otherwise.
            int toInt() const
            {
                return isNull() ? 0 : obj->toInt();
            }

            //! Returns object->toInt() if the referenced object is not nullptr, defaultValue otherwise.
            int toInt(int defaultValue) const
            {
                return isNull() ? defaultValue : obj->toInt();
            }

            //! Return object->toUInt() if the referenced object is not nullptr, 0 otherwise.
            unsigned int toUInt() const
            {
                return isNull() ? 0u : obj->toUInt();
            }

            //! Return object->toUInt() if the referenced object is not nullptr, defaultValue otherwise.
            unsigned int toUInt(unsigned int defaultValue) const
            {
                return isNull() ? defaultValue : obj->toUInt();
            }

            //! Returns object->toString() if the referenced object is not nullptr, "" otherwise.
            std::string toString() const
            {
                return isNull() ? "" : obj->toString();
            }

            //! Returns object->toString() if the referenced object is not nullptr, defaultValue otherwise.
            std::string toString(const std::string& defaultValue) const
            {
                return isNull() ? defaultValue : obj->toString();
            }

            //! Tries to convert object to given Type; returns nullptr if object is nullptr or not of given type.
            template <class _T2> _T2* toType() const
            {
                return isNull() ? nullptr : dynamic_cast<_T2*>(obj);
            }
            template <class _T2> _T2* castTo() const
            {
                return dynamic_cast<_T2*>(this->get());
            }
    };


    /*! Init the EScript system and all registered libraries.
	Has to be called once before any script can be executed */
    void init();

    /*! Calls the the given init(...) function with the SGLOBALS-Object (static global namespace).
	\note Can be used manually as an alternative to the automatic registerLibraryForInitialization-way. */
    void initLibrary(libInitFunction initFunction);

    //! (internal) Get the static super global namespace.
    Namespace* getSGlobals();

    namespace Win32Lib
    {
        void init(EScript::Namespace* o);

        void setClipboard(const std::string& s);
        std::string getClipboard();
    }

    namespace StdLib
    {
        void init(EScript::Namespace* o);

        /*!
        * Loads and executes the script from file __filename__ if the file has not already been loaded by this function.
        * Returns the return value;
        * \note May throw 'Object *' on error!
        */
        ObjRef loadOnce(Runtime& runtime, const std::string& filename);

        //! formatted output
        void print_r(Object* o, int maxLevel = 7, int level = 1);

        // returns "WINDOWS" | "MAX OS" | "LINUX" | "UNIX" | "UNKNOWN"
        std::string getOS();
    }

    namespace MathLib
    {
        void init(EScript::Namespace* o);
    }

    namespace IOLib
    {
        void init(EScript::Namespace* o);
    }

    /**
    * JSON support for EScript
    * [static-helper]
    */
    struct JSON
    {
        static void toJSON(std::ostringstream& out, Object* obj, bool formatted = true, int level = 0);
        static std::string toJSON(Object* obj, bool formatted = true);
        static Object* parseJSON(const std::string& s);
    };




    extern const std::string ES_UNKNOWN_IDENTIFIER;

    identifierId stringToIdentifierId(const std::string& s);
    const std::string& identifierIdToString(identifierId id);

    //! (internal)
    hashvalue _hash(const std::string& s);


    //! Numerical representation of a string constant.
    class StringId
    {
        private:
            uint32_t id;

        public:
            static const std::string& toString(const uint32_t value)
            {
                return identifierIdToString(value);
            }

            StringId() : id(0)
            {
            }
            explicit StringId(uint32_t _id) : id(_id)
            {
            }
            /*implicit*/ StringId(const std::string& str) : id(stringToIdentifierId(str))
            {
            }
            StringId(const StringId&) = default;
            StringId(StringId&&) = default;

            uint32_t getValue() const
            {
                return id;
            }
            const std::string& toString() const
            {
                return identifierIdToString(id);
            }

            StringId& operator=(const StringId&) = default;
            StringId& operator=(StringId&&) = default;
            StringId& operator=(const std::string& str)
            {
                id = stringToIdentifierId(str);
                return *this;
            }
            bool empty() const
            {
                return id == 0;
            }

            bool operator==(const StringId& other) const
            {
                return id == other.id;
            }
            bool operator!=(const StringId& other) const
            {
                return id != other.id;
            }
            bool operator<(const StringId& other) const
            {
                return id < other.id;
            }
    };
}

namespace std
{
    template <> struct hash<EScript::StringId>
    {
        size_t operator()(const EScript::StringId& id) const noexcept
        {
            return id.getValue();
        }
    };
}

namespace EScript
{
    //! Internal representation of an object's attribute
    class Attribute
    {
        public:
            typedef uint8_t flag_t;
            static const flag_t NORMAL_ATTRIBUTE = 0;
            // 0...normal	1...const
            static const flag_t CONST_BIT = (1 << 0);
            // 0...public	1...private
            static const flag_t PRIVATE_BIT = (1 << 1);
            // 0...objAttr	1...typeAttr
            static const flag_t TYPE_ATTR_BIT = (1 << 2);
            // 0...normal	1...init
            static const flag_t INIT_BIT = (1 << 3);
            // 0...normal	1...reference    //! \todo Unused! Check if still useful.
            static const flag_t REFERENCE_BIT = (1 << 4);
            // 0...normal	1...override
            static const flag_t OVERRIDE_BIT = (1 << 5);

            static const flag_t ASSIGNMENT_RELEVANT_BITS = CONST_BIT | PRIVATE_BIT | REFERENCE_BIT;

        private:
            ObjRef value;
            flag_t properties;

        public:
            explicit Attribute(flag_t _properties = NORMAL_ATTRIBUTE) : properties(_properties)
            {
            }
            /*implicit*/ Attribute(const ObjPtr& _value, flag_t _properties = NORMAL_ATTRIBUTE)
            : value(_value.get()), properties(_properties)
            {
            }
            /*implicit*/ Attribute(const ObjRef& _value, flag_t _properties = NORMAL_ATTRIBUTE)
            : value(_value.get()), properties(_properties)
            {
            }
            /*implicit*/ Attribute(Object* _value, flag_t _properties = NORMAL_ATTRIBUTE)
            : value(_value), properties(_properties)
            {
            }
            /*implicit*/ Attribute(const Attribute& e) : value(e.value), properties(e.properties)
            {
            }

            bool getProperty(flag_t f) const
            {
                return (properties & f) > 0;
            }
            flag_t getProperties() const
            {
                return properties;
            }

            Object* getValue() const
            {
                return value.get();
            }
            bool isConst() const
            {
                return properties & CONST_BIT;
            }
            bool isInitializable() const
            {
                return properties & INIT_BIT;
            }
            bool isNull() const
            {
                return value.isNull();
            }
            bool isNotNull() const
            {
                return value.isNotNull();
            }
            explicit operator bool() const
            {
                return bool(value);
            }

            bool isObjAttribute() const
            {
                return !(properties & TYPE_ATTR_BIT);
            }

            bool isTypeAttribute() const
            {
                return properties & TYPE_ATTR_BIT;
            }

            bool isPrivate() const
            {
                return properties & PRIVATE_BIT;
            }

            //! \todo Unused! Check if still useful.
            bool isReference() const
            {
                return properties & REFERENCE_BIT;
            }

            bool isOverriding() const
            {
                return properties & OVERRIDE_BIT;
            }

            void setValue(Object* v)
            {
                value = v;
            }

            void set(Object* v, flag_t f)
            {
                value = v, properties = f;
            }

            Attribute& operator=(const Attribute& e)
            {
                set(e.value.get(), e.properties);
                return *this;
            }
    };

    template <class Obj_t> struct _DefaultReleaseHandler
    {
        static void release(Obj_t* t)
        {
            delete t;
        }
    };

    /*! (Non virtual) base class for reference counting.
	@p Obj_t  Should be the new class itself.
	@p ObjReleaseHandler_T	A class which has the function 'static void release(Ojb_t *)'
		for releasing (deleting or storing) counted objects.	*/
    template <class Obj_t, class ObjReleaseHandler_T = _DefaultReleaseHandler<Obj_t>>
    class EReferenceCounter
    {
        private:
            int refCounter;

        public:
            EReferenceCounter() : refCounter(0)
            {
            }
            EReferenceCounter(const EReferenceCounter&) = delete;
            EReferenceCounter(EReferenceCounter&&) = delete;

            //! Default destructor
            ~EReferenceCounter()
            {
            }

            //! Return the current number of references to this object.
            inline int countReferences() const
            {
                return refCounter;
            }

            //! Increase the reference counter of @p o.
            static inline void addReference(Obj_t* o)
            {
                if(o != nullptr)
                    ++o->refCounter;
            }

            //! Decrease the reference counter of @p o. If the counter is <= 0, the object is released.
            static inline void removeReference(Obj_t* o)
            {
                if(o != nullptr && (--o->refCounter) == 0)
                    ObjReleaseHandler_T::release(o);
            }
            //! Decrease the reference counter of @p o. The object is never released.
            static inline void decreaseReference(Obj_t* o)
            {
                if(o != nullptr)
                    --o->refCounter;
            }
            EReferenceCounter& operator=(const EReferenceCounter&) = delete;
            EReferenceCounter& operator=(EReferenceCounter&&) = delete;
    };


    struct _TypeIds
    {
        static const internalTypeId_t TYPE_UNKNOWN = 0xff;

        // call by value types <0x10
        static const internalTypeId_t TYPE_NUMBER = 0x01;
        static const internalTypeId_t TYPE_STRING = 0x02;
        static const internalTypeId_t TYPE_BOOL = 0x03;

        // 0x10 >= mixed
        static const internalTypeId_t TYPE_FUNCTION = 0x10;
        static const internalTypeId_t TYPE_USER_FUNCTION = 0x11;
        static const internalTypeId_t TYPE_FN_BINDER = 0x12;
        static const internalTypeId_t TYPE_ARRAY = 0x13;
        static const internalTypeId_t TYPE_VOID = 0x14;
        static const internalTypeId_t TYPE_IDENTIFIER = 0x15;
        static const internalTypeId_t TYPE_TYPE = 0x16;
    };


    class Object : public EReferenceCounter<Object, ObjectReleaseHandler>
    {
        public:
            static const char* getClassName()
            {
                return "Object";
            }

            virtual const char* getTypeName() const
            {
                return getClassName();
            }

        public:
            static Type* getTypeObject();
            static void init(EScript::Namespace& globals);

        public:
            //! Default constructor; Sets the type to Object::getTypeObject()
            Object();
            Object(Type* type);
            virtual ~Object();

            friend std::ostream& operator<<(std::ostream& s, EScript::Object* o)
            {
                return o == nullptr ?
                       s << "nullptr" :
                       s << "[" << o->getTypeName() << ":" << static_cast<void*>(o) << ":" << o->countReferences() << "]";
            }
            //! Return a clone of the object if the type is call-by-value and the object itthisObj otherwise.
            Object* getRefOrCopy();
            virtual Object* clone() const;
            virtual StringId hash() const;

            /*! ---o
                \note For camparing objects, never use this function directly but use isEqual(...) instead.
                      Otherwise scripted '=='-member functions are not supported. */
            virtual bool rt_isEqual(Runtime& rt, const ObjPtr& other);

            //! Compare two Objects using the '=='-member function
            bool isEqual(Runtime& rt, const ObjPtr& o);

            /*! If this is an Object which is passed ...
                 -  call-by-value, this functions returns true if the given object's Type is the same as this' type
                    and isEqual returns true.
                 - 	call-by-reference, this function returns true if the given object and this are the same Object. */
            bool isIdentical(Runtime& rt, const ObjPtr& other);

            //! For internal use only.
            virtual internalTypeId_t _getInternalTypeId() const
            {
                return _TypeIds::TYPE_UNKNOWN;
            }

            /*! If this object is the value of an attribute marked as reference, and a new value is assigned to this attribute,
                this function is called instead of setting the new object as value for the attribute.
                This makes only sense for reference objects like NumberRef.
                \todo Unused! Check if still useful.
                ---o	*/
            virtual void _assignValue(ObjPtr value);

        protected:
            ERef<Type> typeRef;

        public:
            inline Type* getType() const
            {
                return typeRef.get();
            }

            virtual bool isA(const Type* type) const;

        public:
            /*! ---o (internal)
                Get access to an Attribute stored at this Object.
                \note Should not be called directly. Use get(Local)Attribute(...) instead.
                \note Has to be overridden if an Object type should support user defined attributes.
            */
            virtual Attribute* _accessAttribute(const StringId& id, bool localOnly);

            /*! ---o (internal)
            This function is called by the runtime after a new Object has been created in the script using "new". The
                execution takes place after the Object itthisObj has been created, but before the first scripted constructor is executed.
                Extended attribute initializations can be performed here.
                \note Has to be overridden if an Object type should support user defined attributes. */
            virtual void _initAttributes(Runtime& rt);

            /*! Get the locally stored attribute with the given id.
                If the attribute is not found, the resulting attribute references nullptr.
                \code
                    const Attribute & attr = obj->getLocalAttribute("attrName");
                    if(attr.isNull()) ...
            */
            const Attribute& getLocalAttribute(const StringId& id) const;

            /*! Get the attribute with the given id. The attribute can be stored locally or be accessible by the object's type.
                If the attribute is not found, the resulting attribute references nullptr.
                \code
                    const Attribute & attr = obj->getAttribute("doesNotExist");
                    if(attr.isNull()) ...
            */
            const Attribute& getAttribute(const StringId& id) const;
            const Attribute& getAttribute(const char* c_str) const
            {
                return getAttribute(StringId(c_str));
            }

            /*!	---o
                Try to set the value of an object attribute.
                Returns false if the attribute can not be set.
                \note Has to be overridden if an Object type should support user defined attributes. */
            virtual bool setAttribute(const StringId& id, const Attribute& attr);
            bool setAttribute(const char* c_str, const Attribute& attr)
            {
                return setAttribute(StringId(c_str), attr);
            }

            /*! ---o
                Collect all attributes in a map; used for debugging. */
            virtual void collectLocalAttributes(std::unordered_map<StringId, Object*>&)
            {
            }

        public:
            virtual std::string toString() const;
            virtual int toInt() const;
            virtual unsigned int toUInt() const;
            virtual double toDouble() const;
            virtual float toFloat() const;
            virtual bool toBool() const;
            virtual std::string toDbgString() const;
    };

    /*! Wrapper object for an StringId
	[Identifier]  ---|> [Object]  */
    class Identifier : public Object
    {
            ES_PROVIDES_TYPE_NAME(Identifier)
        public:
            static Type* getTypeObject();
            static void init(EScript::Namespace& globals);
            static Identifier* create(StringId id);
            static Identifier* create(const std::string& s);

        private:
            Identifier(const StringId& _id);

        public:
            virtual ~Identifier()
            {
            }

        private:
            StringId id;

        public:
            StringId& operator*()
            {
                return id;
            }
            StringId operator*() const
            {
                return id;
            }

            StringId getId() const
            {
                return id;
            }

            Identifier* clone() const override
            {
                return create(id);
            }
            bool rt_isEqual(Runtime& rt, const ObjPtr& o) override;
            std::string toString() const override
            {
                return id.toString();
            }
            StringId hash() const override
            {
                return id;
            }
            internalTypeId_t _getInternalTypeId() const override
            {
                return _TypeIds::TYPE_IDENTIFIER;
            }
    };

    class ObjectReleaseHandler
    {
        public:
            static void release(Object* obj);
    };


    class AttributeContainer
    {
        public:
            typedef std::unordered_map<StringId, Attribute> attributeMap_t;
            typedef attributeMap_t::iterator iterator;
            typedef attributeMap_t::const_iterator const_iterator;
            typedef attributeMap_t::size_type size_type;
            typedef attributeMap_t::value_type value_type;

        private:
            void operator=(const AttributeContainer& other);

        private:
            attributeMap_t attributes;

        public:
            explicit AttributeContainer(const AttributeContainer& other);
            explicit AttributeContainer()
            {
            }
            ~AttributeContainer()
            {
            }

            Attribute* accessAttribute(const StringId& id)
            {
                const attributeMap_t::iterator f = attributes.find(id);
                return f == attributes.end() ? nullptr : &f->second;
            }
            const_iterator begin() const
            {
                return attributes.begin();
            }
            const_iterator end() const
            {
                return attributes.end();
            }
            iterator begin()
            {
                return attributes.begin();
            }
            iterator end()
            {
                return attributes.end();
            }
            void clear()
            {
                attributes.clear();
            }
            void cloneAttributesFrom(const AttributeContainer& other);
            const attributeMap_t& getAttributes() const
            {
                return attributes;
            }
            void collectAttributes(std::unordered_map<StringId, Object*>& attrs);
            void initAttributes(Runtime& rt);
            void setAttribute(const StringId& id, const Attribute& attr)
            {
                attributes[id] = attr;
            }
            size_t size() const
            {
                return attributes.size();
            }
    };

    class Type : public Object
    {
        public:
            typedef uint16_t flag_t;

        private:
            flag_t flags;
            AttributeContainer attributes;
            ERef<Type> baseType;

        public:
            static const char* getClassName()
            {
                return "Type";
            }

            virtual const char* getTypeName() const
            {
                return getClassName();
            }


        public:
            static Type* getTypeObject();
            static void init(EScript::Namespace& globals);

        public:
            Type();
            Type(Type* baseType);
            Type(Type* baseType, Type* typeOfType);
            virtual ~Type();
            Object* clone() const override;
            internalTypeId_t _getInternalTypeId() const override
            {
                return _TypeIds::TYPE_TYPE;
            }

            /*! @name Attributes
            \note for an information about the using directives, see
                http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/topic/com.ibm.xlcpp8l.doc/language/ref/overload_member_fn_base_derived.htm
        */
        public:
            //! Get only the typeAttributes.
            void collectTypeAttributes(std::unordered_map<StringId, Object*>& attrs) const;
            //! Get only the objectAttributes.
            void collectObjAttributes(std::unordered_map<StringId, Object*>& attrs) const;

            void copyObjAttributesTo(Object* instance);

            //! Used by instances of this type get the value of an inherited typeAttribute.
            Attribute* findTypeAttribute(const StringId& id);

            using Object::_accessAttribute;
            using Object::setAttribute;

            Attribute* _accessAttribute(const StringId& id, bool localOnly) override;

            bool setAttribute(const StringId& id, const Attribute& attr) override;

            void collectLocalAttributes(std::unordered_map<StringId, Object*>& attrs) override;

        public:
            void allowUserInheritance(bool b)
            {
                setFlag(FLAG_ALLOWS_USER_INHERITANCE, b);
            }
            bool allowsUserInheritance() const
            {
                return getFlag(FLAG_ALLOWS_USER_INHERITANCE);
            }

            Type* getBaseType() const
            {
                return baseType.get();
            }

            bool hasBase(const Type* type) const;
            bool isBaseOf(const Type* type) const;

            static const flag_t FLAG_CALL_BY_VALUE = 1 << 0;
            static const flag_t FLAG_CONTAINS_OBJ_ATTRS = 1 << 1;
            static const flag_t FLAG_ALLOWS_USER_INHERITANCE = 1 << 2;

            bool getFlag(flag_t f) const
            {
                return (flags & f) > 0;
            }
            void setFlag(flag_t f, bool b = true)
            {
                b ? flags |= f : flags -= (flags & f);
            }
            flag_t getFlags() const
            {
                return flags;
            }
    };

    class ExtObject : public Object
    {
        public:
            static const char* getClassName()
            {
                return "ExtObject";
            }

            virtual const char* getTypeName() const
            {
                return getClassName();
            }

        public:
            static Type* getTypeObject();
            static void init(EScript::Namespace& globals);

        private:
            AttributeContainer objAttributes;

        protected:
            ExtObject(const ExtObject& other);

        public:
            static ExtObject* create();
            ExtObject();
            ExtObject(Type* type);
            virtual ~ExtObject()
            {
            }

            Object* clone() const override;

        public:
            using Object::_accessAttribute;
            using Object::setAttribute;
            Attribute* _accessAttribute(const StringId& id, bool localOnly) override;
            void _initAttributes(Runtime& rt) override;
            bool setAttribute(const StringId& id, const Attribute& attr) override;
            void collectLocalAttributes(std::unordered_map<StringId, Object*>& attrs) override;
            void cloneAttributesFrom(const ExtObject* obj);
    };

    class Namespace : public ExtObject
    {
        public:
            static const char* getClassName()
            {
                return "Namespace";
            }

            virtual const char* getTypeName() const
            {
                return getClassName();
            }

        public:
            static Type* getTypeObject();
            static void init(EScript::Namespace& globals);

            Namespace() : ExtObject()
            {
            }
            Namespace(Type* type) : ExtObject(type)
            {
            }
            virtual ~Namespace()
            {
            }

            Namespace* clone() const override;
    };

    class Logger : public EReferenceCounter<Logger>
    {
    public:
        enum level_t
        {
            LOG_ALL = 0,
            LOG_DEBUG = 1,
            LOG_INFO = 2,
            LOG_PEDANTIC_WARNING = 3,
            LOG_WARNING = 4,
            LOG_ERROR = 5,
            LOG_FATAL = 6,
            LOG_NONE = 10
        };
        Logger(level_t _minLevel = LOG_ALL, level_t _maxLevel = LOG_NONE) : minLevel(_minLevel), maxLevel(_maxLevel)
        {
        }
        virtual ~Logger()
        {
        }

        void debug(const std::string& msg)
        {
            log(LOG_DEBUG, msg);
        }
        void error(const std::string& msg)
        {
            log(LOG_ERROR, msg);
        }
        void fatal(const std::string& msg)
        {
            log(LOG_FATAL, msg);
        }
        level_t getMinLevel() const
        {
            return minLevel;
        }
        level_t getMaxLevel() const
        {
            return maxLevel;
        }
        void info(const std::string& msg)
        {
            log(LOG_INFO, msg);
        }
        void log(level_t l, const std::string& msg)
        {
            if(testLevel(l))
                doLog(l, msg);
        }
        void pedantic(const std::string& msg)
        {
            log(LOG_PEDANTIC_WARNING, msg);
        }
        void setMinLevel(level_t l)
        {
            minLevel = l;
        }
        void setMaxLevel(level_t l)
        {
            maxLevel = l;
        }
        void warn(const std::string& msg)
        {
            log(LOG_WARNING, msg);
        }

    private:
        bool testLevel(level_t t) const
        {
            return static_cast<int>(t) >= static_cast<int>(minLevel) && static_cast<int>(t) <= static_cast<int>(maxLevel);
        }

        virtual void doLog(level_t l, const std::string& msg) = 0;

        level_t minLevel;
        level_t maxLevel;
    };


    /*! [LoggerGroup] ---|>[Logger]
		|
		--------> [Logger*]		*/
    class LoggerGroup : public Logger
    {
    public:
        LoggerGroup(level_t _minLevel = LOG_ALL, level_t _maxLevel = LOG_NONE) : Logger(_minLevel, _maxLevel)
        {
        }
        virtual ~LoggerGroup()
        {
        }

        void addLogger(const std::string& name, Logger* logger);
        bool removeLogger(const std::string& name);
        void clearLoggers();
        Logger* getLogger(const std::string& name);

    private:
        void doLog(level_t l, const std::string& msg) override;
        typedef std::map<std::string, _CountedRef<Logger>> loggerRegistry_t;
        loggerRegistry_t loggerRegistry;
    };

    /*! [StdLogger] ---|>[Logger]
		|
		--------> std::ostream		*/
    class StdLogger : public Logger
    {
    public:
        StdLogger(std::ostream& stream, level_t _minLevel = LOG_ALL, level_t _maxLevel = LOG_NONE)
        : Logger(_minLevel, _maxLevel), out(stream)
        {
        }
        virtual ~StdLogger()
        {
        }

    private:
        void doLog(level_t l, const std::string& msg) override;
        std::ostream& out;
    };


    /**
 * Array of fixed size for EScript::Objects (via ObjRef or ObjPtr).
 * \note This array is especially optimized for sizes of < 3 (typical number of parameters). For those sizes,
 *	the benchmark indicates that it is a good deal faster than a std::vector.
 */
    template <typename _T>
    class _ObjArray
    {
        public:
            typedef _T* iterator;
            typedef const _T* const_iterator;
            typedef std::size_t size_type;
            typedef _ObjArray<_T> thisObj_t;

        private:
            size_type paramCount;
            _T internalParams[2];
            _T* params;

        private:
            _ObjArray& operator=(const _ObjArray& other);

        public:

            _ObjArray() : paramCount(0), params(nullptr)
            {
            }
            _ObjArray(ObjPtr p1) : paramCount(1), params(internalParams)
            {
                params[0] = p1;
            }
            _ObjArray(ObjPtr p1, ObjPtr p2) : paramCount(2), params(internalParams)
            {
                params[0] = p1, params[1] = p2;
            }
            _ObjArray(ObjPtr p1, ObjPtr p2, ObjPtr p3) : paramCount(3), params(new _T[3])
            {
                params[0] = p1, params[1] = p2, params[2] = p3;
            }
            _ObjArray(ObjPtr p1, ObjPtr p2, ObjPtr p3, ObjPtr p4) : paramCount(4), params(new _T[4])
            {
                params[0] = p1, params[1] = p2, params[2] = p3, params[3] = p4;
            }
            explicit _ObjArray(size_type _paramCount)
            : paramCount(_paramCount), params(paramCount > 2 ? new _T[paramCount] : internalParams)
            {
            }
            explicit _ObjArray(const _ObjArray& other)
            : paramCount(other.paramCount), params(paramCount > 2 ? new _T[paramCount] : internalParams)
            {
                if(paramCount > 0)
                    std::copy(other.begin(), other.end(), begin());
            }

            ~_ObjArray()
            {
                if(paramCount > 2)
                    delete[] params;
            }

            inline void set(size_type i, ObjPtr v)
            {
                params[i] = v;
            }
            inline void emplace(size_type i, ObjRef&& v)
            {
                params[i] = std::move(v);
            }

            inline size_type count() const
            {
                return paramCount;
            }
            inline size_type size() const
            {
                return paramCount;
            }
            inline bool empty() const
            {
                return paramCount == 0;
            }

            inline ObjPtr operator[](size_type i) const
            {
                return i < paramCount ? params[i] : nullptr;
            }
            inline ObjPtr get(size_type i) const
            {
                return i < paramCount ? params[i] : nullptr;
            }
            inline iterator begin()
            {
                return params;
            }
            inline const_iterator begin() const
            {
                return params;
            }
            inline iterator end()
            {
                return params + size();
            }
            inline const_iterator end() const
            {
                return params + size();
            }
    };




    class Runtime : public ExtObject
    {
    public:
        static const char* getClassName()
        {
            return "Runtime";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    private:
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

    private:
        std::unique_ptr<RuntimeInternals> internals;

    public:
        Runtime();
        virtual ~Runtime();

    public:
        Namespace* getGlobals() const;

        bool assignToAttribute(ObjPtr obj, StringId attrId, ObjPtr value);

    public:
        ObjPtr getCallingObject() const;

        //! \note throws an exception (Object *) on failure
        ObjRef executeFunction(const ObjPtr& fun, const ObjPtr& callingObject, const ParameterValues& params);

        //! \note throws an exception (Object *) on failure
        ObjRef createInstance(const EPtr<Type>& type, const ParameterValues& params);

        //! \note throws an exception (Object *) on failure
        void yieldNext(YieldIterator& yIt);

        size_t getStackSize() const;
        size_t _getStackSizeLimit() const;
        void _setStackSizeLimit(const size_t limit);

    public:
        DEPRECATED bool assertNormalState() const;

        void info(const std::string& s);
        void warn(const std::string& s);

        ObjRef fetchAndClearExitResult();

        bool checkNormalState() const;

        /*! Creates an exception object including current stack info and
			sets the internal state to STATE_EXCEPTION. Does NOT throw a C++ exception. */
        void setException(const std::string& s);

        /*! Annotates the given Exception with the current stack info and set the internal state
			to STATE_EXCEPTION. Does NOT throw a C++ exception. */
        void setException(Exception* e);

        //! (internal) Like 'setException' but does NOT annotate the given exception but just uses it.
        void _setExceptionState(ObjRef e);

        void _setExitState(ObjRef e);

        /**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setException(...)
		 */
        void throwException(const std::string& s, Object* obj = nullptr);

    public:
        void enableLogCounting();
        void disableLogCounting();

        std::string getCurrentFile() const;
        int getCurrentLine() const;
        uint32_t getLogCounter(Logger::level_t level) const;
        LoggerGroup* getLogger() const
        {
            return logger.get();
        }
        Logger::level_t getLoggingLevel()
        {
            return logger->getMinLevel();
        }
        std::string getStackInfo();
        std::string getLocalStackInfo();

        void log(Logger::level_t l, const std::string& s)
        {
            logger->log(l, s);
        }
        void resetLogCounter(Logger::level_t level);

        void setAddStackInfoToExceptions(bool b);
        void setLoggingLevel(Logger::level_t level)
        {
            logger->setMinLevel(level);
        }
        void setTreatWarningsAsError(bool b);

    private:
        _CountedRef<LoggerGroup> logger;
    };



    //! Runtime value. Used for function return values and stack values.
    class RtValue
    {
    public:
        enum valueType_t
        {
            VOID_VALUE,
            OBJECT_PTR,
            BOOL,
            UINT32,
            NUMBER,
            IDENTIFIER,
            LOCAL_STRING_IDX,
            FUNCTION_CALL_CONTEXT,
            UNDEFINED
        } valueType;

    private:
        union value_t
        {
            Object* value_obj;
            bool value_bool;
            uint32_t value_uint32;
            double value_number;
            StringId value_indentifier;
            uint32_t value_localStringIndex;
            FunctionCallContext* value_fcc;
            uint64_t raw;

            value_t() : raw(0)
            {
            }
            value_t(const value_t& other)
            {
                raw = other.raw;
            }
            value_t& operator=(const value_t& other)
            {
                static_assert(sizeof(raw) == sizeof(value_t), "'raw' must cover the whole union.");
                raw = other.raw;
                return *this;
            }
            ~value_t()
            {
            }
            void reset()
            {
                raw = 0;
            }
        } value;
        RtValue(const valueType_t type) : valueType(type)
        {
        }

    public:
        static RtValue createLocalStringIndex(const uint32_t idx)
        {
            RtValue v(LOCAL_STRING_IDX);
            v.value.value_localStringIndex = idx;
            return v;
        }

        static RtValue createFunctionCallContext(FunctionCallContext* fcc)
        {
            RtValue v(FUNCTION_CALL_CONTEXT);
            v.value.value_fcc = fcc;
            return v;
        }

        RtValue() : valueType(UNDEFINED)
        {
        }

        template <class T, class = typename std::enable_if<std::is_same<T, bool>::value>::type>
        RtValue(T b) : valueType(BOOL)
        {
            value.value_bool = b;
        }
        RtValue(const StringId& id) : valueType(IDENTIFIER)
        {
            value.value_indentifier = id;
        }
        RtValue(const double& v) : valueType(NUMBER)
        {
            value.value_number = v;
        }
        RtValue(const float& v) : valueType(NUMBER)
        {
            value.value_number = v;
        }
        RtValue(const int& v) : valueType(NUMBER)
        {
            value.value_number = v;
        }
        RtValue(const uint32_t& v) : valueType(UINT32)
        {
            value.value_uint32 = v;
        }
        RtValue(const std::string& s);
        RtValue(const char* s);
        RtValue(std::nullptr_t) : valueType(VOID_VALUE)
        {
        }

        RtValue(Object* obj)
        {
            if(obj == nullptr)
            {
                valueType = VOID_VALUE;
            }
            else
            {
                valueType = OBJECT_PTR;
                value.value_obj = obj;
                Object::addReference(value.value_obj);
            }
        }
        RtValue(const ObjPtr& obj)
        {
            if(obj == nullptr)
            {
                valueType = VOID_VALUE;
            }
            else
            {
                valueType = OBJECT_PTR;
                value.value_obj = obj.get();
                Object::addReference(value.value_obj);
            }
        }
        RtValue(const RtValue& other) : valueType(other.valueType), value(other.value)
        {
            if(valueType == OBJECT_PTR)
                Object::addReference(value.value_obj);
        }
        RtValue(const ObjRef& obj)
        {
            if(obj == nullptr)
            {
                valueType = VOID_VALUE;
            }
            else
            {
                valueType = OBJECT_PTR;
                value.value_obj = obj.get();
                Object::addReference(value.value_obj);
            }
        }
        RtValue(ObjRef&& obj)
        {
            if(obj == nullptr)
            {
                valueType = VOID_VALUE;
            }
            else
            {
                valueType = OBJECT_PTR;
                value.value_obj = obj.detach();
            }
        }

        RtValue(RtValue&& other) : valueType(other.valueType), value(other.value)
        {
            other.valueType = UNDEFINED;
        }

    public:
        ~RtValue()
        {
            if(valueType == OBJECT_PTR)
                Object::removeReference(value.value_obj);
        }
        RtValue& operator=(const RtValue& other)
        {
            // new value is object
            if(other.valueType == OBJECT_PTR)
            {
                Object::addReference(other.value.value_obj);
            }
            // old value is object
            if(valueType == OBJECT_PTR)
            {
                Object::removeReference(value.value_obj);
            }
            valueType = other.valueType;
            value = other.value;
            return *this;
        }
        RtValue& operator=(RtValue&& other)
        {
            if(valueType == OBJECT_PTR)
                Object::removeReference(value.value_obj);
            valueType = other.valueType;
            value = other.value;
            other.valueType = UNDEFINED;
            return *this;
        }

        //! Detach the object without checking the type or changing the object's reference counter.
        Object* _detachObject()
        {
            valueType = RtValue::UNDEFINED;
            return value.value_obj;
        }
        Object* getObject() const
        {
            return valueType == OBJECT_PTR ? value.value_obj : nullptr;
        }

        //! Access the value of a specific type without checking if the type is correct.
        bool _getBool() const
        {
            return value.value_bool;
        }
        FunctionCallContext* _getFCC() const
        {
            return value.value_fcc;
        }
        StringId _getIdentifier() const
        {
            return value.value_indentifier;
        }
        Object* _getObject() const
        {
            return value.value_obj;
        }
        uint32_t _getLocalStringIndex() const
        {
            return value.value_localStringIndex;
        }
        double _getNumber() const
        {
            return value.value_number;
        }
        uint32_t _getUInt32() const
        {
            return value.value_uint32;
        }

        bool isFunctionCallContext() const
        {
            return valueType == FUNCTION_CALL_CONTEXT;
        }
        bool isIdentifier() const
        {
            return valueType == IDENTIFIER;
        }
        bool isLocalString() const
        {
            return valueType == LOCAL_STRING_IDX;
        }
        bool isNumber() const
        {
            return valueType == NUMBER;
        }
        bool isObject() const
        {
            return valueType == OBJECT_PTR;
        }
        bool isUint32() const
        {
            return valueType == UINT32;
        }
        bool isUndefined() const
        {
            return valueType == UNDEFINED;
        }
        bool isVoid() const
        {
            return valueType == VOID_VALUE;
        }

        bool toBool() const
        {
            return valueType == BOOL ? value.value_bool : toBool2();
        }

    private:
        // expensive part of toBool()
        bool toBool2() const;

    public:
        std::string toDbgString() const;

        /*! Convert the value to an object;
			\note Do not use if the type can be LOCAL_STRING_IDX or FUNCTION_CALL_CONTEXT as this can't be properly converted!*/
        Object* _toObject() const;
    };




    //! Add a type Function attribute to @p type with given name.
    void declareFunction(Type* type, StringId nameId, _functionPtr fn);
    void declareFunction(Type* type, const char* name, _functionPtr fn);
    void declareFunction(Type* type, StringId nameId, int minParamCount, int maxParamCount, _functionPtr fn);
    void declareFunction(Type* type, const char* name, int minParamCount, int maxParamCount, _functionPtr fn);
    void declareConstant(Type* type, StringId nameId, const RtValue& value);
    void declareConstant(Type* type, const char* name, const RtValue& value);

    void declareFunction(Namespace* nameSpace, StringId nameId, _functionPtr fn);
    void declareFunction(Namespace* nameSpace, const char* name, _functionPtr fn);
    void declareFunction(Namespace* nameSpace, StringId nameId, int minParamCount, int maxParamCount, _functionPtr fn);
    void declareFunction(Namespace* nameSpace, const char* name, int minParamCount, int maxParamCount, _functionPtr fn);
    void declareConstant(Namespace* nameSpace, StringId nameId, const RtValue& value);
    void declareConstant(Namespace* nameSpace, const char* name, const RtValue& value);

    //! Adds a function as object attribute (it is copied to each instance)
    void declareObjectFunction(Type* type, const char* name, _functionPtr fn);

    void initPrintableName(Type* type, const std::string& printableName);
    void initPrintableName(ExtObject* type, const std::string& printableName);

    void markAttributeAsObjectAttribute(Type* type, StringId nameId);
    void markAttributeAsObjectAttribute(Type* type, const char* name);

    void copyAttributeAsAlias(Type* type, const char* originalName, const char* aliasName);





    namespace _Internals
    {
        //! (internal) Non-inline part of @a assertParamCount
        void assertParamCount_2(Runtime& runtime, int paramCount, int min, int max);
        //! (internal) Non-inline part of @a assertType.
        void assertType_throwError(Runtime& runtime, const ObjPtr& obj, const char* className);
    }

    /*!
 * Check if the number of given parameters is in the given range (min <= number <= max).
 * A range value of <0 accepts an arbitrary number of parameters.
 * If too many parameters are given, a warning message is shown.
 * If too few parameter are given, a runtime error is thrown.
 */
    inline void assertParamCount(Runtime& runtime, const ParameterValues& params, int min = -1, int max = -1)
    {
        const int paramCount = static_cast<int>(params.count());
        if((paramCount < min && min >= 0) || ((paramCount > max) && max >= 0))
            _Internals::assertParamCount_2(runtime, paramCount, min, max);
    }
    inline void assertParamCount(Runtime& runtime, const size_t paramCount, int min = -1, int max = -1)
    {
        const int paramCountI = static_cast<int>(paramCount);
        if((paramCountI < min && min >= 0) || ((paramCountI > max) && max >= 0))
            _Internals::assertParamCount_2(runtime, paramCountI, min, max);
    }

    /*!
 * Try to cast the given object to the specified type.
 * If the object is not of the appropriate type, a runtime error is thrown.
 */
    template <class T> static T* assertType(Runtime& runtime, const ObjPtr& obj)
    {
        T* t = dynamic_cast<T*>(obj.get());
        if(t == nullptr)
            _Internals::assertType_throwError(runtime, obj, T::getClassName());
        return t;
    }

    ObjRef callMemberFunction(Runtime& rt, ObjPtr obj, StringId fnNameId, const ParameterValues& params);
    ObjRef callFunction(Runtime& rt, Object* function, const ParameterValues& params);

    /*! Compile and execute the given code and return the result.
	\note may throw 'std::exception' or 'Object *' on failure. */
    ObjRef _eval(Runtime& runtime, const CodeFragment& code, const std::unordered_map<StringId, ObjRef>& staticVars);

    /*! Compile and execute the given code. If the code could be executed without an exception, (true,result) is returned;
	if an exception (of type Object*) occured (false,exception) is returned and the error message is sent to the runtime's logger.
	@param fileId Name used to identify the executed code in exception messages and stack traces; the default is '[inline]'
	@return (success, result)
*/
    std::pair<bool, ObjRef>
    eval(Runtime& runtime, const StringData& code, const StringId& fileId, const std::unordered_map<StringId, ObjRef>& staticVars);
    std::pair<bool, ObjRef> eval(Runtime& runtime, const StringData& code, const StringId& fileId = StringId());

    //! @return (success, result)
    std::pair<bool, ObjRef> executeStream(Runtime& runtime, std::istream& stream);

    //! @return result (mmay throw an exception)
    ObjRef _loadAndExecute(Runtime& runtime, const std::string& filename, const std::unordered_map<StringId, ObjRef>& staticVars);

    //! @return (success, result)
    std::pair<bool, ObjRef> loadAndExecute(Runtime& runtime, const std::string& filename);
    std::pair<bool, ObjRef>
    loadAndExecute(Runtime& runtime, const std::string& filename, const std::unordered_map<StringId, ObjRef>& staticVars);

    //! creates and throws an Exception objects.
    void throwRuntimeException(const std::string& what);

    template <> inline bool convertTo<bool>(Runtime&, ObjPtr src)
    {
        return src.toBool();
    }

    template <> double convertTo<double>(Runtime& rt, ObjPtr src);
    template <> inline float convertTo<float>(Runtime& rt, ObjPtr src)
    {
        return static_cast<float>(convertTo<double>(rt, src));
    }
    template <> inline long long convertTo<long long>(Runtime& rt, ObjPtr src)
    {
        return static_cast<long long>(convertTo<double>(rt, src));
    }
    template <> inline unsigned long long convertTo<unsigned long long>(Runtime& rt, ObjPtr src)
    {
        return static_cast<unsigned long long>(convertTo<double>(rt, src));
    }
    template <> inline long convertTo<long>(Runtime& rt, ObjPtr src)
    {
        return static_cast<long>(convertTo<double>(rt, src));
    }
    template <> inline unsigned long convertTo<unsigned long>(Runtime& rt, ObjPtr src)
    {
        return static_cast<unsigned long>(convertTo<double>(rt, src));
    }
    template <> inline int convertTo<int>(Runtime& rt, ObjPtr src)
    {
        return static_cast<int>(convertTo<double>(rt, src));
    }
    template <> inline unsigned int convertTo<unsigned int>(Runtime& rt, ObjPtr src)
    {
        return static_cast<unsigned int>(convertTo<double>(rt, src));
    }
    template <> inline short convertTo<short>(Runtime& rt, ObjPtr src)
    {
        return static_cast<short>(convertTo<double>(rt, src));
    }
    template <> inline unsigned short convertTo<unsigned short>(Runtime& rt, ObjPtr src)
    {
        return static_cast<unsigned short>(convertTo<double>(rt, src));
    }

    template <> inline std::string convertTo<std::string>(Runtime&, ObjPtr src)
    {
        return src.toString();
    }



    /*! Factories for types that are not results of unwanted implicit type conversions. */
    Array* create(const std::vector<Object*>&);
    String* create(const StringData&);
    String* create(const std::string&);
    String* create(const char*);
    Void* create(std::nullptr_t);
    Object* create(const ObjRef& obj);

    /*!	For primitive types that can be the result of an implicit type conversion,
	a template function is used that requires the exact type.
	This allows detecting errors if EScript::create(SomeType*) is
	not specified for "SomeType*" (or the corresponding include is missing) and
	the implicit cast would then result in creating an object of type Bool.	*/
    namespace _Internals
    {
        Number* createNumber(double v);
        Number* createNumber(float v);
        Number* createNumber(int v);
        Number* createNumber(uint32_t v);
        Bool* createBool(bool v);
    }

    template <class T, class = typename std::enable_if<std::is_same<T, bool>::value>::type> Bool* create(T value)
    {
        return _Internals::createBool(value);
    }

    template <class T,
              class = typename std::enable_if<std::is_same<T, double>::value || std::is_same<T, float>::value
                                              || std::is_same<T, int>::value || std::is_same<T, uint32_t>::value>::type>
    Number* create(T value)
    {
        return _Internals::createNumber(value);
    }

    /*! EScript::value(someValue) returns a RtValue wrapping 'someValue'. If 'someValue' can not be represented as
	RtValue directly, a corresponding EScript object is created by calling EScript::create(someValue).
	\note All specific create functions must be present before this file is included in order to be found by the
		compiler. \see http://gcc.gnu.org/gcc-4.7/porting_to.html (Name lookup changes)
	*/
    template <typename source_t>
    inline RtValue value(source_t obj, typename std::enable_if<std::is_convertible<source_t, RtValue>::value>::type* = nullptr)
    {
        return RtValue(obj);
    }

    template <typename source_t>
    inline RtValue value(source_t obj, typename std::enable_if<!std::is_convertible<source_t, RtValue>::value>::type* = nullptr)
    {
        return EScript::create(obj);
    }


    class Collection : public Object
    {
    public:
        static const char* getClassName()
        {
            return "Collection";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    public:
        static Type* getTypeObject();
        static void init(Namespace& globals);

        Collection(Type* type = nullptr) : Object(type ? type : getTypeObject())
        {
        }

        virtual ~Collection()
        {
        }

        virtual Object* getValue(ObjPtr key);
        virtual void setValue(ObjPtr key, ObjPtr value);
        virtual void clear();
        virtual size_t count() const;
        virtual Iterator* getIterator();
        virtual Object* rt_findValue(Runtime& runtime, ObjPtr value);
        virtual bool rt_contains(Runtime& runtime, ObjPtr value);
        virtual Object* rt_reduce(Runtime& runtime, ObjPtr function, ObjPtr initialValue, const ParameterValues& additionalValues);
        virtual Object* rt_map(Runtime& runtime, ObjPtr function, const ParameterValues& additionalValues);
        virtual Object* rt_extract(Runtime& runtime, StringId functionId, bool decision = true);
        bool rt_isEqual(Runtime& runtime, const ObjPtr& other) override;
    };

    class Iterator : public Object
    {
    public:
        static const char* getClassName()
        {
            return "Iterator";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    private:
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

        Iterator(Type* type = nullptr) : Object(type ? type : getTypeObject())
        {
        }
        virtual ~Iterator()
        {
        }
    
        virtual Object* key()
        {
            return nullptr;
        }
        virtual Object* value()
        {
            return nullptr;
        }
        virtual void reset()
        {
        }
        virtual void next()
        {
        }
        virtual bool end()
        {
            return true;
        }
    };

    class Array : public Collection
    {
    public:
        static const char* getClassName()
        {
            return "Array";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }


    public:
        typedef ObjRef value_type;

        typedef std::vector<value_type> container_t;
        typedef container_t::iterator iterator;
        typedef container_t::pointer pointer;
        typedef container_t::const_pointer const_pointer;
        typedef container_t::const_iterator const_iterator;
        typedef container_t::reference reference;
        typedef container_t::const_reference const_reference;
        typedef container_t::size_type size_type;

        typedef std::ptrdiff_t difference_type;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    private:
        static std::stack<Array*> pool;

        Array(Type* type = nullptr) : Collection(type ? type : getTypeObject())
        {
        }

        void init(const ParameterValues& p);
        void init(size_t num, Object* const* objs);
        void init(size_t num, char** strings);

    public:
        static Array* create(Type* type = nullptr);
        static Array* create(const ParameterValues& p, Type* type = nullptr);
        static Array* create(size_t num, Object* const* objs, Type* type = nullptr);
        static Array* create(size_t num, char** strings, Type* type = nullptr);

        template <class Collection_T> static Array* create(const Collection_T& collection)
        {
            ERef<Array> a = Array::create();
            a->reserve(collection.size());
            for(auto& elem : collection)
                a->pushBack(EScript::create(elem));
            return a.detachAndDecrease();
        }

        static void release(Array* b);
        virtual ~Array()
        {
        }

    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

    private:
        container_t data;

    public:
        iterator begin()
        {
            return data.begin();
        }
        const_iterator begin() const
        {
            return data.begin();
        }
        iterator end()
        {
            return data.end();
        }
        const_iterator end() const
        {
            return data.end();
        }
        reverse_iterator rbegin()
        {
            return data.rbegin();
        }
        const_reverse_iterator rbegin() const
        {
            return data.rbegin();
        }
        reverse_iterator rend()
        {
            return data.rend();
        }
        const_reverse_iterator rend() const
        {
            return data.rend();
        }

        container_t& operator*()
        {
            return data;
        }
        const container_t& operator*() const
        {
            return data;
        }

        ObjRef& at(size_t idx)
        {
            return data.at(idx);
        }
        const ObjRef& at(size_t idx) const
        {
            return data.at(idx);
        }
        void append(Collection* c);
        Object* back() const
        {
            return empty() ? nullptr : (*(end() - 1)).get();
        }
        bool empty() const
        {
            return data.empty();
        }
        iterator erase(iterator i)
        {
            return data.erase(i);
        }
        iterator erase(iterator i, iterator j)
        {
            return data.erase(i, j);
        }
        Object* front() const
        {
            return empty() ? nullptr : (*(begin())).get();
        }
        Object* get(size_t index) const
        {
            return index < data.size() ? data.at(index).get() : nullptr;
        }
        std::string implode(const std::string& delimiter = ";");
        void popBack()
        {
            data.pop_back();
        }
        void popFront()
        {
            data.erase(begin());
        }
        void pushBack(const ObjPtr& obj)
        {
            if(!obj.isNull())
                data.push_back(obj);
        }
        void pushFront(const ObjPtr& obj)
        {
            if(!obj.isNull())
                data.insert(begin(), obj.get());
        }
        void removeIndex(size_t index);
        void reserve(size_t capacity);
        void resize(size_t newSize);
        void reverse();
        void rt_filter(Runtime& runtime, ObjPtr function);
        //! returns -1 if not found
        int rt_indexOf(Runtime& runtime, ObjPtr search, size_t begin = 0);
        size_t rt_removeValue(Runtime& runtime, const ObjPtr value, const int limit = -1, const size_t begin = 0);
        void rt_sort(Runtime& runtime, Object* function = nullptr, bool reverseOrder = false);
        size_t size() const
        {
            return data.size();
        }
        ERef<Array> slice(int startIndex, int length) const;
        void splice(int startIndex, int length, Array* replacement);
        void swap(Array* other);

    public:
        //!	[ArrayIterator] ---|> [Iterator] ---|> [Object]
        class ArrayIterator : public Iterator
        {
        public:
            static const char* getClassName()
            {
                return "ArrayIterator";
            }

            virtual const char* getTypeName() const
            {
                return getClassName();
            }

        private:
        public:
            ArrayIterator(Array* ar);
            virtual ~ArrayIterator();
            Object* key() override;
            Object* value() override;
            void reset() override;
            void next() override;
            bool end() override;

        private:
            ERef<Array> arrayRef;
            size_t index;
        };

        Object* getValue(ObjPtr key) override;
        void setValue(ObjPtr key, ObjPtr value) override;
        size_t count() const override;
        ArrayIterator* getIterator() override;
        void clear() override;

        Object* clone() const override;
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_ARRAY;
        }
    };


    //! [Map] ---|> [Collection] ---|> [Object]
    class Map : public Collection
    {
    public:
        static const char* getClassName()
        {
            return "Map";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    public:
        //!	MapEntry:   (key,value)
        struct MapEntry
        {
            MapEntry()
            {
            }
            MapEntry(const ObjPtr& _key, const ObjPtr& _value) : key(_key), value(_value)
            {
            }
            MapEntry(const MapEntry& other) : key(other.key), value(other.value)
            {
            }
            MapEntry& operator=(const MapEntry& other)
            {
                if(this != &other)
                {
                    key = other.key;
                    value = other.value;
                }
                return *this;
            }
            ObjRef key;
            ObjRef value;
        };
        typedef std::map<std::string, MapEntry> container_t;
        typedef container_t::iterator iterator;
        typedef container_t::pointer pointer;
        typedef container_t::const_pointer const_pointer;
        typedef container_t::const_iterator const_iterator;
        typedef container_t::reference reference;
        typedef container_t::const_reference const_reference;
        typedef container_t::size_type size_type;

        typedef std::ptrdiff_t difference_type;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    public:
        static Map* create();
        static Map* create(const std::unordered_map<StringId, Object*>&);

        template <class Map_T> static Map* create(const Map_T& m)
        {
            ERef<Map> eM = Map::create();
            for(auto& keyValue : m)
                eM->setValue(EScript::create(keyValue.first), EScript::create(keyValue.second));
            return eM.detachAndDecrease();
        }

        Map(Type* type = nullptr) : Collection(type ? type : getTypeObject())
        {
        }
        virtual ~Map()
        {
        }

    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

    private:
        container_t data;

    public:
        container_t& operator*()
        {
            return data;
        }
        const container_t& operator*() const
        {
            return data;
        }

        iterator begin()
        {
            return data.begin();
        }
        const_iterator begin() const
        {
            return data.begin();
        }
        iterator end()
        {
            return data.end();
        }
        const_iterator end() const
        {
            return data.end();
        }
        reverse_iterator rbegin()
        {
            return data.rbegin();
        }
        const_reverse_iterator rbegin() const
        {
            return data.rbegin();
        }
        reverse_iterator rend()
        {
            return data.rend();
        }
        const_reverse_iterator rend() const
        {
            return data.rend();
        }

        bool empty() const
        {
            return data.empty();
        }
        size_type erase(const std::string& key)
        {
            return data.erase(key);
        }
        Object* getValue(const std::string& key);
        Object* getKeyObject(const std::string& key);
        void merge(Collection* c, bool overwrite = true);
        void rt_filter(Runtime& runtime, ObjPtr function, const ParameterValues& additionalValues);
        void unset(ObjPtr key);
        void swap(Map* other);

        class MapIterator : public Iterator
        {
        public:
            static const char* getClassName()
            {
                return "MapIterator";
            }

            virtual const char* getTypeName() const
            {
                return getClassName();
            }

        private:
        public:
            MapIterator(Map* ar);
            virtual ~MapIterator()
            {
            }

            Object* key() override;
            Object* value() override;
            void reset() override;
            void next() override;
            bool end() override;

        private:
            ERef<Map> mapRef;
            container_t::iterator it;
        };
        void clear() override;
        size_t count() const override;
        MapIterator* getIterator() override;
        Object* getValue(ObjPtr key) override;
        void setValue(ObjPtr key, ObjPtr value) override;
        Object* clone() const;
    };

    //! [Number] ---|> [Object]
    class Number : public Object
    {
    public:
        static const char* getClassName()
        {
            return "Number";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    private:
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);
        static Number* create(double value);
        static void release(Number* n);

        /**
		 * Test if the two parameters are essentially equal as defined in
		 * the given reference by Knuth.
		 *
		 * @param u First floating point parameter.
		 * @param v Second floating point parameter.
		 * @return @c true if both floating point values are essentially
		 * equal, @c false otherwise.
		 * @see Donald E. Knuth: The art of computer programming.
		 * Volume II: Seminumerical algorithms. Addison-Wesley, 1969.
		 * Page 128, Equation (24).
		 */
        inline static bool matches(const float u, const float v)
        {
            return std::abs(v - u) <= std::numeric_limits<float>::epsilon() * std::min(std::abs(u), std::abs(v));
        }

        explicit Number(double value);
        virtual ~Number()
        {
        }

        double& operator*()
        {
            return value;
        }
        double operator*() const
        {
            return value;
        }

        /**
		 * Convert the number to a string configurable string representation.
		 *
		 * @param precision Number of digits after the decimal point.
		 * @param scientific Switch for scientific or fixed-point notation.
		 * @param width Number of digits for padding.
		 * @param fill Character to use for padding.
		 * @return Formatted string representation of the number.
		 */
        std::string format(std::streamsize precision = 3, bool scientific = true, std::streamsize width = 0, char fill = '0') const;

        //! Floating point symmetric modulo operation
        double modulo(const double m) const;

        double getValue() const
        {
            return value;
        }
        void setValue(double _value)
        {
            value = _value;
        }

        //! ---|> [Object]
        Object* clone() const override
        {
            return create(value);
        }

        std::string toString() const override;
        double toDouble() const override
        {
            return value;
        }
        bool rt_isEqual(Runtime& rt, const ObjPtr& o) override;
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_NUMBER;
        }

    private:
        double value;
    };

    //! [Bool] ---|> [Object]
    class Bool : public Object
    {
    public:
        static const char* getClassName()
        {
            return "Bool";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    private:
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);
        static Bool* create(bool value);
        static void release(Bool* b);

        explicit Bool(bool _value) : Object(getTypeObject()), value(_value)
        {
        }
        virtual ~Bool()
        {
        }

        void setValue(bool b)
        {
            value = b;
        }

        bool& operator*()
        {
            return value;
        }
        bool operator*() const
        {
            return value;
        }

        //! ---|> [Object]
        Object* clone() const override
        {
            return create(value);
        }
        std::string toString() const override
        {
            return value ? "true" : "false";
        }
        bool toBool() const override
        {
            return value;
        }
        double toDouble() const override
        {
            return value ? 1 : 0;
        }
        bool rt_isEqual(Runtime&, const ObjPtr& o) override
        {
            return value == o.toBool(false);
        }
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_BOOL;
        }

    private:
        bool value;
    };


    //! [StringData]
    class StringData
    {
        //! internals
        struct Data
        {
            std::string s;
            int referenceCounter;
            enum dataType_t
            {
                // the string consists of bytes without special semantic
                RAW,
                // the string consists only of ascii-characters (<128)
                ASCII,
                // the string contains of an unknown number of unknown code points
                UNKNOWN_UNICODE,
                // the string contains of a known number of code points
                UNICODE_WITH_LENGTH,
                // the string contains of a known number of code points and contains
                UNICODE_WITH_JUMTABLE
                //  a jump table for random accesses
            } dataType;
            std::unique_ptr<std::vector<size_t>> jumpTable;//!< jumpTable[i] := strPos of codePoint( (i+1)*JUMP_TABLE_STEP_SIZE)
            size_t numCodePoints;

            Data(const std::string& _s, dataType_t t) : s(_s), referenceCounter(0), dataType(t), numCodePoints(0)
            {
            }
            Data(const char* c, size_t size, dataType_t t)
            : s(c, size), referenceCounter(0), dataType(t), numCodePoints(0)
            {
            }
            Data(Data&&) = default;
            Data(const Data&) = delete;
            void initJumpTable();
        };
        static Data* createData(const std::string& s);
        static Data* createData(const char* c, size_t size);
        static void releaseData(Data* data);

        void setData(Data* newData);
        Data* data;
        static Data* getEmptyData();
        static std::stack<Data*> dataPool;

        void initJumpTable() const;

    public:
        StringData() : data(getEmptyData())
        {
            ++data->referenceCounter;
        }
        explicit StringData(const std::string& s) : data(createData(s))
        {
            ++data->referenceCounter;
        }
        explicit StringData(const char* c, size_t size) : data(createData(c, size))
        {
            ++data->referenceCounter;
        }
        StringData(const StringData& other) : data(other.data)
        {
            ++data->referenceCounter;
        }

        ~StringData()
        {
            if((--data->referenceCounter) <= 0)
                releaseData(data);
        }
        /*! Returns the byte index of the given codePointIdx in the utf8 encoded string.
			If the codePoint is invalid, std::string::npos is returned. */
        size_t codePointToBytePos(const size_t codePointNr) const;
        bool empty() const
        {
            return str().empty();
        }

        uint32_t getCodePoint(const size_t codePointIdx) const;
        size_t getDataSize() const
        {
            return str().length();
        }
        size_t getNumCodepoints() const;
        std::string getSubStr(const size_t codePointStart, const size_t numCodePoints) const;

        bool beginsWith(const std::string& subj, const size_t codePointStart = 0) const;
        size_t find(const std::string& subj, const size_t codePointStart = 0) const;
        size_t rFind(const std::string& subj, const size_t codePointStart = std::string::npos) const;

        bool operator==(const StringData& other) const
        {
            return (data == other.data) || (str() == other.str());
        }
        StringData& operator=(const StringData& other)
        {
            setData(other.data);
            return *this;
        }
        StringData& operator=(const std::string& s)
        {
            setData(createData(s));
            return *this;
        }
        void set(const StringData& other)
        {
            setData(other.data);
        }
        void set(const std::string& s)
        {
            setData(createData(s));
        }
        const std::string& str() const
        {
            return data->s;
        }
    };

    namespace IO
    {
        enum entryType_t
        {
            TYPE_NOT_FOUND = -1,
            TYPE_FILE = 0,
            TYPE_DIRECTORY = 1,
            TYPE_UNKNOWN = 2
        };
        struct EntryInfo
        {
            uint64_t fileSize;
            entryType_t type;
            uint32_t mTime;
            uint32_t cTime;
            EntryInfo() : fileSize(0), type(TYPE_UNKNOWN), mTime(0), cTime(0)
            {
            }
        };

        class AbstractFileSystemHandler;

        /*! Set a new fileSystemHandler responsible for all io-operations.
	The old handler is deleted.	*/
        void setFileSystemHandler(AbstractFileSystemHandler* handler);
        AbstractFileSystemHandler* getFileSystemHandler();

        StringData loadFile(const std::string& filename);
        void saveFile(const std::string& filename, const std::string& content, bool overwrite = true);

        /*! @param filename
 *	@return file modification Time	*/
        uint32_t getFileMTime(const std::string& filename);

        /*!	@param filename
 *	@return  IO::entryType (\see IOBase.h)	*/
        entryType_t getEntryType(const std::string& filename);

        /*!	@param filename
 *	@return filsize in byte.	*/
        uint64_t getFileSize(const std::string& filename);

        /*!
 * @param dirname
 * @param flags		1 ... Files
 *					2 ... Directories
 *					4 ... Recurse Subdirectories
 * @throw std::ios_base::failure on failure.	*/
        std::vector<std::string> getFilesInDir(const std::string& dirname, uint8_t flags);

        std::string dirname(const std::string& filename);

        /*! Remove "." and ".." from the inputPath if possible.
 *	\example
 *		foo						-> foo
 *		/var/bla/				-> /var/bla/
 *		bla/./foo				-> bla/foo
 *		bla/foo/./../../bar/.	-> bar
 *		///						-> /
 *		//						-> /
 *		../../foo/bla/..		-> ../../foo		*/
        std::string condensePath(const std::string& inputPath);

        class AbstractFileSystemHandler
        {
        protected:
            AbstractFileSystemHandler()
            {
            }

        public:
            virtual ~AbstractFileSystemHandler()
            {
            }

            virtual void deleteFile(const std::string&)
            {
                throw std::ios_base::failure("unsupported operation");
            }
            /*!	---o
	 * @param dirname
	 * @param flags		1 ... Files
	 *					2 ... Directories
	 *					4 ... Recurse Subdirectories
	 * @throw std::ios_base::failure on failure.	*/
            virtual std::vector<std::string> dir(const std::string& /*dirname*/, uint8_t /*flags*/)
            {
                throw std::ios_base::failure("unsupported operation");
            }

            virtual entryType_t getEntryType(const std::string& path)
            {
                return getEntryInfo(path).type;
            }

            virtual EntryInfo getEntryInfo(const std::string&)
            {
                throw std::ios_base::failure("unsupported operation");
            }

            virtual uint32_t getFileCTime(const std::string& path)
            {
                return getEntryInfo(path).cTime;
            }

            virtual uint32_t getFileMTime(const std::string& path)
            {
                return getEntryInfo(path).mTime;
            }

            virtual uint64_t getFileSize(const std::string& path)
            {
                return getEntryInfo(path).fileSize;
            }

            virtual void makeDir(const std::string&)
            {
                throw std::ios_base::failure("unsupported operation");
            }

            virtual StringData loadFile(const std::string&)
            {
                throw std::ios_base::failure("unsupported operation");
            }

            virtual void saveFile(const std::string&, const std::string& /*data*/, bool /*overwrite*/)
            {
                throw std::ios_base::failure("unsupported operation");
            }
        };

        //! DefaultFileSystemHandler ---|> AbstractFileSystemHandler
        class DefaultFileSystemHandler : public AbstractFileSystemHandler
        {
        public:
            DefaultFileSystemHandler()
            {
            }
            virtual ~DefaultFileSystemHandler()
            {
            }

            //! ---|> AbstractFileSystemHandler
            std::vector<std::string> dir(const std::string&, uint8_t) override;

            //! ---|> AbstractFileSystemHandler
            EntryInfo getEntryInfo(const std::string&) override;

            //! ---|> AbstractFileSystemHandler
            StringData loadFile(const std::string&) override;

            //! ---|> AbstractFileSystemHandler
            void saveFile(const std::string&, const std::string& /*data*/, bool /*overwrite*/) override;
        };
    }

    //! [String] ---|> [Object]
    class String : public Object
    {
    public:
        static const char* getClassName()
        {
            return "String";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    private:
    private:
        explicit String(const StringData& _sData) : Object(getTypeObject()), sData(_sData)
        {
        }

        //! internal helper
        static StringData objToStringData(Object* obj);

    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

        static String* create(const std::string& s)
        {
            return create(StringData(s));
        }
        static String* create(const StringData& sData);
        static void release(String* b);

        virtual ~String()
        {
        }

        StringData& operator*()
        {
            return sData;
        }
        const std::string& operator*() const
        {
            return sData.str();
        }

        void appendString(const std::string& _s)
        {
            sData.set(sData.str() + _s);
        }
        bool empty() const
        {
            return sData.empty();
        }
        size_t length() const
        {
            return sData.getNumCodepoints();
        }
        size_t getDataSize() const
        {
            return sData.getDataSize();
        }

        const StringData& _getStringData() const
        {
            return sData;
        }
        StringData& _getStringData()
        {
            return sData;
        }
        const std::string& getString() const
        {
            return sData.str();
        }
        void setString(const std::string& _s)
        {
            sData.set(_s);
        }
        void setString(const StringData& _sData)
        {
            sData.set(_sData);
        }

        //! ---|> [Object]
        Object* clone() const override
        {
            return create(sData);
        }
        std::string toString() const override
        {
            return getString();
        }
        double toDouble() const override;
        int toInt() const override;
        bool rt_isEqual(Runtime& rt, const ObjPtr& o) override;
        std::string toDbgString() const override;
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_STRING;
        }

    private:
        StringData sData;
    };


    //! [Void] ---|> [Type] ---|> [Object]
    class Void : public Type
    {
    public:
        static const char* getClassName()
        {
            return "Void";
        }

        virtual const char* getTypeName() const
        {
            return getClassName();
        }

    private:
    public:
        static Void* get();

        //! ---|> [Object]
        Object* clone() const override;
        bool toBool() const override;
        bool rt_isEqual(Runtime& rt, const ObjPtr& o) override;
        std::string toString() const override;
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_VOID;
        }

    protected:
        virtual ~Void();
        Void();
    };



    class Debug
    {
    public:
        static void registerObj(Object* v);
        static void unRegisterObj(Object* v);
        static void showObjects();
        static void clearObjects();
    };

    //! Simple container used to store the code of a UserFunction
    class CodeFragment
    {
        StringId filename;
        StringData data;
        size_t start, length;

    public:
        CodeFragment() : start(0), length(0)
        {
        }
        CodeFragment(const StringId& _filename, const StringData& fullCode)
        : filename(_filename), data(fullCode), start(0), length(fullCode.getDataSize())
        {
        }
        CodeFragment(const StringId& _filename, const StringData& _data, const size_t _start, const size_t _length)
        : filename(_filename), data(_data), start(_start), length(_length)
        {
        }
        CodeFragment(const CodeFragment& other, const size_t _start, const size_t _length)
        : filename(other.filename), data(other.data), start(_start), length(_length)
        {
        }

        bool empty() const
        {
            return length == 0;
        }
        std::string getCodeString() const
        {
            return data.str().substr(start, length);
        }
        std::string getFilename() const
        {
            return filename.toString();
        }
        const std::string& getFullCode() const
        {
            return data.str();
        }
        size_t getLength() const
        {
            return length;
        }
        size_t getStartPos() const
        {
            return start;
        }
    };


    /*! [Instruction]
	Work in progress!	*/
    class Instruction
    {
    public:
        enum type_t
        {
            I_ASSIGN_ATTRIBUTE,// -2
            I_ASSIGN_LOCAL,// -1
            I_ASSIGN_VARIABLE,// -1
            I_CALL,// -2+x +1
            I_CREATE_INSTANCE,// -1+x +1
            I_DUP,// +1
            I_FIND_VARIABLE,// +2
            I_GET_ATTRIBUTE,// -1 +1
            I_GET_VARIABLE,// +1
            I_GET_LOCAL_VARIABLE,// +1
            I_INIT_CALLER,// -x +0
            I_JMP,// +-0
            I_JMP_IF_SET,// -1
            I_JMP_ON_TRUE,// -1
            I_JMP_ON_FALSE,// -1
            I_NOT,// -1 +1
            I_POP,// -1
            I_PUSH_BOOL,// +1
            I_PUSH_ID,// +1
            I_PUSH_FUNCTION,// +1
            I_PUSH_NUMBER,// +1
            I_PUSH_STRING,// +1
            I_PUSH_UINT,// +1
            I_PUSH_UNDEFINED,// +1
            I_PUSH_VOID,// +1
            I_RESET_LOCAL_VARIABLE,// -x
            I_SET_ATTRIBUTE,// -3
            I_SET_EXCEPTION_HANDLER,// +-0
            I_SYS_CALL,// +-?
            I_YIELD,// -1
            I_UNDEFINED,
            I_SET_MARKER// +-0
        };
        //! if a jump target is >= JMP_TO_MARKER_OFFSET, the target is a marker and not an address.
        static const uint32_t JMP_TO_MARKER_OFFSET = 0x100000;
        //! A jump to this address always ends the current function. \todo assure that no IntructionBlock can have so many Instructions
        static const uint32_t INVALID_JUMP_ADDRESS = 0x0FFFFF;

        std::string toString(const InstructionBlock& ctxt) const;

        type_t getType() const
        {
            return type;
        }

        uint32_t getValue_uint32() const
        {
            return data.value_uint32;
        }
        void setValue_uint32(const uint32_t v)
        {
            data.value_uint32 = v;
        }

        double getValue_Number() const
        {
            return data.value_number;
        }
        void setValue_Number(double v)
        {
            data.value_number = v;
        }

        StringId getValue_Identifier() const
        {
            return StringId(data.value_identifier);
        }
        void setValue_Identifier(StringId v)
        {
            data.value_identifier = v.getValue();
        }

        bool getValue_Bool() const
        {
            return data.value_bool;
        }
        void setValue_Bool(bool v)
        {
            data.value_bool = v;
        }

        std::pair<uint32_t, uint32_t> getValue_uint32Pair() const
        {
            return data.value_uint32Pair;
        }
        void setValue_uint32Pair(uint32_t v1, uint32_t v2)
        {
            data.value_uint32Pair = std::make_pair(v1, v2);
        }

        static Instruction createAssignAttribute(const StringId& varName);
        static Instruction createAssignLocal(const uint32_t localVarIdx);
        static Instruction createAssignVariable(const StringId& varName);
        static Instruction createCall(const uint32_t numParams);
        static Instruction createCreateInstance(const uint32_t numParams);
        static Instruction createDup()
        {
            return Instruction(I_DUP);
        }
        static Instruction createFindVariable(const StringId& id);
        static Instruction createGetAttribute(const StringId& id);
        static Instruction createGetLocalVariable(const uint32_t localVarIdx);
        static Instruction createGetVariable(const StringId& id);
        static Instruction createInitCaller(const uint32_t numSuperParams);
        static Instruction createJmp(const uint32_t markerId);
        static Instruction createJmpIfSet(const uint32_t markerId);
        static Instruction createJmpOnTrue(const uint32_t markerId);
        static Instruction createJmpOnFalse(const uint32_t markerId);
        static Instruction createNot()
        {
            return Instruction(I_NOT);
        }
        static Instruction createPop()
        {
            return Instruction(I_POP);
        }
        static Instruction createPushBool(const bool value);
        static Instruction createPushId(const StringId& id);
        static Instruction createPushFunction(const uint32_t functionIdx);
        static Instruction createPushNumber(const double value);
        static Instruction createPushString(const uint32_t stringIndex);
        static Instruction createPushUInt(const uint32_t value);
        static Instruction createPushUndefined()
        {
            return Instruction(I_PUSH_UNDEFINED);
        }
        static Instruction createPushVoid()
        {
            return Instruction(I_PUSH_VOID);
        }
        static Instruction createResetLocalVariable(const uint32_t localVarIdx);
        static Instruction createSetAttribute(const StringId& id);
        static Instruction createSetExceptionHandler(const uint32_t markerId);
        static Instruction createSetMarker(const uint32_t markerId);
        static Instruction createSysCall(const uint32_t fnIdx, const uint32_t numParams);
        static Instruction createYield()
        {
            return Instruction(I_YIELD);
        }

        int getLine() const
        {
            return line;
        }
        void setLine(const int l)
        {
            line = l;
        }

    private:
        Instruction(type_t _type) : type(_type), line(-1)
        {
        }
        type_t type;
        union value_t
        {
            double value_number;
            size_t value_numParams;
            uint32_t value_identifier;
            uint32_t value_uint32;
            bool value_bool;
            std::pair<uint32_t, uint32_t> value_uint32Pair;
            uint64_t raw;
            value_t() : raw(0)
            {
                static_assert(sizeof(raw) == sizeof(value_t), "'raw' must cover the whole union.");
            }
            value_t(value_t&& other) : raw(other.raw)
            {
            }
            value_t(const value_t& other) : raw(other.raw)
            {
            }
            value_t& operator=(value_t&& other)
            {
                raw = other.raw;
                return *this;
            }
            value_t& operator=(const value_t& other)
            {
                raw = other.raw;
                return *this;
            }
        } data;
        int line;
    };


    //! Collection of (assembler-)instructions and the corresponding data.
    class InstructionBlock
    {
        std::vector<StringId> localVariables;
        std::vector<std::string> stringConstants;//! \todo --> StringData
        std::vector<Instruction> instructions;
        std::vector<ObjRef> internalFunctions;//! UserFunction
        // flags...
    public:
        InstructionBlock();

        void addInstruction(const Instruction& newInstruction)
        {
            instructions.push_back(newInstruction);
        }
        void addInstruction(const Instruction& newInstruction, int line)
        {
            instructions.push_back(newInstruction);
            instructions.back().setLine(line);
        }
        uint32_t registerInternalFunction(const ObjPtr& userFunction)
        {
            internalFunctions.push_back(userFunction);
            return static_cast<uint32_t>(internalFunctions.size() - 1);
        }

        uint32_t declareString(const std::string& str)
        {
            stringConstants.push_back(str);
            return static_cast<uint32_t>(stringConstants.size() - 1);
        }
        uint32_t declareLocalVariable(const StringId& name)
        {
            localVariables.push_back(name);
            return static_cast<uint32_t>(localVariables.size() - 1);
        }
        const Instruction& getInstruction(const size_t index) const
        {
            return instructions[index];
        }
        int getLine(const size_t index) const
        {
            return index < instructions.size() ? instructions[index].getLine() : -1;
        }

        StringId getLocalVarName(const size_t index) const;
        const std::vector<StringId>& getLocalVariables() const
        {
            return localVariables;
        }

        size_t getNumLocalVars() const
        {
            return localVariables.size();
        }
        size_t getNumInstructions() const
        {
            return instructions.size();
        }
        std::string getStringConstant(const uint32_t index) const
        {
            return index <= stringConstants.size() ? stringConstants[index] : "";
        }
        UserFunction* getUserFunction(const uint32_t index) const;

        std::vector<Instruction>& _accessInstructions()
        {
            return instructions;
        }
        const std::vector<Instruction>& getInstructions() const
        {
            return instructions;
        }

        std::string toString() const;
    };





    /*! Compiler
	Input: Syntax tree made of Expressions
	Output: Byte Code made of EAssembler-Instructions */

    class Compiler
    {
    public:
        Compiler(Logger* _logger = nullptr);

        std::pair<ERef<UserFunction>, _CountedRef<StaticData>>
        compile(const CodeFragment& code, const std::vector<StringId>& injectedStaticVarNames);

    public:
        Logger* getLogger() const
        {
            return logger.get();
        }

    private:
        _CountedRef<Logger> logger;

    public:
        /*! (static,internal)
			- Replaces the markers inside the assembly by jump addresses.	*/
        static void finalizeInstructions(InstructionBlock& instructions);
        void addExpression(FnCompileContext& ctxt, EPtr<AST::ASTNode> expression) const;
        void addStatement(FnCompileContext& ctxt, EPtr<AST::ASTNode> statement) const;

        void throwError(FnCompileContext& ctxt, const std::string& message) const;

    private:
        void compileASTNode(FnCompileContext& ctxt, EPtr<AST::ASTNode> node) const;
    };

    namespace AST
    {
        //! [ASTNode]  ---|> [EReferenceCounter]
        class ASTNode : public EReferenceCounter<ASTNode>
        {
            ES_PROVIDES_TYPE_NAME(ASTNode)
        public:
            typedef std::vector<ERef<ASTNode>> refArray_t;
            typedef EPtr<ASTNode> ptr_t;
            typedef ERef<ASTNode> ref_t;

            typedef uint8_t nodeType_t;

            static const nodeType_t TYPE_ANNOTATED_STATEMENT = 0x00;
            static const nodeType_t TYPE_BLOCK_EXPRESSION = 0x01;
            static const nodeType_t TYPE_BLOCK_STATEMENT = 0x02;
            static const nodeType_t TYPE_BREAK_STATEMENT = 0x03;
            static const nodeType_t TYPE_CONDITIONAL_EXPRESSION = 0x04;
            static const nodeType_t TYPE_CONTINUE_STATEMENT = 0x05;
            static const nodeType_t TYPE_EXIT_STATEMENT = 0x06;
            static const nodeType_t TYPE_FOREACH_STATEMENT = 0x07;
            static const nodeType_t TYPE_FUNCTION_CALL_EXPRESSION = 0x08;
            static const nodeType_t TYPE_GET_ATTRIBUTE_EXPRESSION = 0x09;
            static const nodeType_t TYPE_IF_STATEMENT = 0x0a;
            static const nodeType_t TYPE_LOGIC_OP_EXPRESSION = 0x0b;
            static const nodeType_t TYPE_LOOP_STATEMENT = 0x0c;
            static const nodeType_t TYPE_RETURN_STATEMENT = 0x0d;
            static const nodeType_t TYPE_SET_ATTRIBUTE_EXPRESSION = 0x0e;
            static const nodeType_t TYPE_SWITCH_STATEMENT = 0x0f;
            static const nodeType_t TYPE_THROW_STATEMENT = 0x10;
            static const nodeType_t TYPE_TRY_CATCH_STATEMENT = 0x11;
            static const nodeType_t TYPE_USER_FUNCTION_EXPRESSION = 0x12;
            static const nodeType_t TYPE_VALUE_BOOL = 0x13;
            static const nodeType_t TYPE_VALUE_FLOATING_POINT = 0x14;
            static const nodeType_t TYPE_VALUE_IDENTIFIER = 0x15;
            static const nodeType_t TYPE_VALUE_STRING = 0x16;
            static const nodeType_t TYPE_VALUE_VOID = 0x17;
            static const nodeType_t TYPE_YIELD_STATEMENT = 0x18;

            virtual ~ASTNode()
            {
            }

            nodeType_t getNodeType() const
            {
                return typeId;
            }
            int getLine() const
            {
                return line;
            }
            bool isExpression() const
            {
                return isExpr;
            }
            void setLine(int l)
            {
                line = l;
            }

        protected:
            ASTNode(nodeType_t _typeId, bool _isExpression, int _line = -1)
            : typeId(_typeId), line(_line), isExpr(_isExpression)
            {
            }
            void convert(nodeType_t _typeId, bool _isExpression)
            {
                typeId = _typeId;
                isExpr = _isExpression;
            }

        private:
            nodeType_t typeId;
            int line;
            bool isExpr;
        };
    }

    inline bool isLocalVarLocation(const varLocation_t& l)
    {
        return l.second >= 0 && l.first == variableType_t::LOCAL_VAR;
    }
    inline bool isStaticVarLocation(const varLocation_t& l)
    {
        return l.second >= 0 && l.first == variableType_t::STATIC_VAR;
    }
    inline bool isValidVarLocation(const varLocation_t& l)
    {
        return l.second >= 0;
    }


    namespace AST
    {
        //! [Block]  ---|> [ASTNode]
        class Block : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(Block)
        public:
            typedef std::deque<ref_t> statementList_t;
            typedef statementList_t::iterator statementCursor;
            typedef statementList_t::const_iterator cStatementCursor;

            static Block* createBlockExpression(int line = -1)
            {
                return new Block(TYPE_BLOCK_EXPRESSION, true, line);
            }
            static Block* createBlockStatement(int line = -1)
            {
                return new Block(TYPE_BLOCK_STATEMENT, false, line);
            }

            virtual ~Block()
            {
            }

            statementList_t& getStatements()
            {
                return statements;
            }
            const statementList_t& getStatements() const
            {
                return statements;
            }

            //! returns false if variable was already declared
            bool declareLocalVar(StringId id)
            {
                return vars.insert(std::make_pair(id, variableType_t::LOCAL_VAR)).second;
            }
            bool declareStaticVar(StringId id)
            {
                return vars.insert(std::make_pair(id, variableType_t::STATIC_VAR)).second;
            }
            const declaredVariableMap_t& getVars() const
            {
                return vars;
            }
            //		bool isLocalVar(StringId id)					{	return vars.count(id)>0 && vars[id]==LOCAL_VAR;	}
            //		bool isDeclaredVar(StringId id)					{	return vars.count(id)>0;	}
            void addStatement(ptr_t s)
            {
                statements.push_back(s);
            }
            bool hasDeclaredVars() const
            {
                return !vars.empty();
            }
            size_t getNumDeclaredVars() const
            {
                return vars.size();
            }

            void convertToStatement()
            {
                convert(TYPE_BLOCK_STATEMENT, false);
            }
            void convertToExpression()
            {
                convert(TYPE_BLOCK_EXPRESSION, true);
            }

        private:
            Block(nodeType_t t, bool expr, int l) : ASTNode(t, expr, l)
            {
            }
            declaredVariableMap_t vars;
            statementList_t statements;
        };
    }

    //! Operator
    class Operator
    {
    public:
        enum associativity_t
        {
            L = 0,
            R = 1
        };

        static const Operator* getOperator(StringId id);
        static const Operator* getOperator(const char* op)
        {
            return getOperator(StringId(op));
        }

        std::string getString() const
        {
            return s;
        }
        StringId getId() const
        {
            return id;
        }
        int getPrecedence() const
        {
            return precedence;
        }
        associativity_t getAssociativity() const
        {
            return associativity;
        }

    private:
        typedef std::unordered_map<StringId, Operator> operatorMap_t;
        static operatorMap_t ops;
        static void declareOperator(int precedence, const std::string& op, associativity_t associativity = L);

        Operator(StringId id, int precedence, const std::string& _s, associativity_t associativity = L);

        StringId id;
        int precedence;
        std::string s;
        associativity_t associativity;
    };

    //! [Token]
    class Token : public EReferenceCounter<Token>
    {
    public:
        static const uint32_t TYPE_ID = 0x00;
        static uint32_t getTypeId()
        {
            return 0x00;
        }

        template <class TokenType_t> static bool isA(Token* t)
        {
            return t == nullptr ? false : ((TokenType_t::getTypeId() & t->typeId) == TokenType_t::getTypeId());
        }
        template <class TokenType_t> static bool isA(const _CountedRef<Token>& t)
        {
            return t.isNull() ? false : ((TokenType_t::getTypeId() & t->typeId) == TokenType_t::getTypeId());
        }
        template <class TokenType_t> static TokenType_t* cast(const _CountedRef<Token>& t)
        {
            return isA<TokenType_t>(t) ? static_cast<TokenType_t*>(t.get()) : nullptr;
        }
        template <class TokenType_t> static TokenType_t* cast(Token* t)
        {
            return isA<TokenType_t>(t) ? static_cast<TokenType_t*>(t) : nullptr;
        }

        static int tokenCount;

        Token(const uint32_t _type = getTypeId()) : typeId(_type), line(0), startingPos(std::string::npos)
        {
            ++tokenCount;
        }
        virtual ~Token()
        {
            tokenCount--;
        }
        virtual std::string toString() const
        {
            return std::string("Token");
        }

        void setLine(int _line)
        {
            line = _line;
        }
        int getLine() const
        {
            return line;
        }

        virtual Token* clone() const
        {
            return new Token;
        }

        uint32_t getType() const
        {
            return typeId;
        }
        const uint32_t typeId;

        void setStaringPos(size_t p)
        {
            startingPos = p;
        }
        size_t getStartingPos() const
        {
            return startingPos;
        }

    private:
        int line;
        size_t startingPos;
    };

    class TIdentifier : public Token
    {
    private:
        StringId id;

    public:
        static const uint32_t TYPE_ID = 0x01 << 0;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }

        TIdentifier(StringId _id) : Token(getTypeId()), id(_id)
        {
        }
        std::string toString() const override
        {
            return id.toString();
        }

        StringId getId() const
        {
            return id;
        }
        Token* clone() const override
        {
            return new TIdentifier(id);
        }
    };

    class TControl : public Token
    {
    public:
        static const uint32_t TYPE_ID = 0x01 << 1;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TControl(const char* _name) : id(_name)
        {
        }
        TControl(StringId _id) : Token(getTypeId()), id(_id)
        {
        }
        std::string toString() const override
        {
            return id.toString();
        }
        StringId getId() const
        {
            return id;
        }
        Token* clone() const override
        {
            return new TControl(id);
        }

    private:
        StringId id;
    };

    struct TEndCommand : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 2;
        TEndCommand() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return ";";
        }
        Token* clone() const override
        {
            return new TEndCommand;
        }

        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
    };

    struct TEndScript : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 3;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TEndScript() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return "EndScript";
        }
        Token* clone() const override
        {
            return new TEndScript;
        }
    };

    struct TStartBlock : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 4;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TStartBlock(AST::Block* _block = nullptr) : Token(getTypeId()), block(_block)
        {
        }
        void setBlock(AST::Block* _block)
        {
            block = _block;
        }
        AST::Block* getBlock() const
        {
            return block.get();
        }
        std::string toString() const override
        {
            return "{";
        }
        Token* clone() const override
        {
            return new TStartBlock(block.get());
        }

    private:
        ERef<AST::Block> block;
    };

    struct TEndBlock : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 5;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TEndBlock() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return "}";
        }
        Token* clone() const override
        {
            return new TEndBlock;
        }
    };

    struct TStartMap : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 6;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TStartMap() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return "_{";
        }
        Token* clone() const override
        {
            return new TStartMap;
        }
    };

    struct TEndMap : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 7;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TEndMap() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return "}_";
        }
        Token* clone() const override
        {
            return new TEndMap;
        }
    };

    struct TMapDelimiter : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 8;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TMapDelimiter() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return "_:_";
        }
        Token* clone() const override
        {
            return new TMapDelimiter;
        }
    };

    struct TColon : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 9;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TColon() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return ":";
        }
        Token* clone() const override
        {
            return new TColon;
        }
    };

    struct TOperator : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 11;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TOperator(const std::string& s, const uint32_t _type = getTypeId()) : Token(_type)
        {
            op = Operator::getOperator(s);
        }
        TOperator(StringId id) : Token(getTypeId())
        {
            op = Operator::getOperator(id);
        }
        TOperator(const Operator* _op) : Token(getTypeId()), op(_op)
        {
        }

        int getPrecedence()
        {
            return op->getPrecedence();
        }
        int getAssociativity()
        {
            return op->getAssociativity();
        }
        const Operator* getOperator() const
        {
            return op;
        }

        std::string toString() const override
        {
            return op->getString();
        }
        Token* clone() const override
        {
            return new TOperator(op);
        }

    private:
        const Operator* op;
    };

    struct TStartBracket : public TOperator
    {
        static const uint32_t TYPE_ID = 0x01 << 12 | TOperator::TYPE_ID;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }

        size_t endBracketIndex;
        TStartBracket() : TOperator("(", getTypeId()), endBracketIndex(0)
        {
        }
        Token* clone() const override
        {
            return new TStartBracket;
        }
    };

    struct TEndBracket : public TOperator
    {// Token: there may be a reason why TEndBrakcet should directly inherit from Token!?!
        static const uint32_t TYPE_ID = 0x01 << 13 | TOperator::TYPE_ID;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TEndBracket() : TOperator(")", getTypeId())
        {
        }
        //	std::string toString()const override		{	return ")";	}
        Token* clone() const override
        {
            return new TEndBracket;
        }
    };

    struct TDelimiter : public TOperator
    {
        static const uint32_t TYPE_ID = 0x01 << 14 | TOperator::TYPE_ID;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TDelimiter() : TOperator(",", getTypeId())
        {
        }
        Token* clone() const override
        {
            return new TDelimiter;
        }
    };

    struct TStartIndex : public TOperator
    {
        static const uint32_t TYPE_ID = 0x01 << 15 | TOperator::TYPE_ID;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TStartIndex() : TOperator("[", getTypeId())
        {
        }
        Token* clone() const override
        {
            return new TStartIndex;
        }
    };

    struct TEndIndex : public TOperator
    {
        static const uint32_t TYPE_ID = 0x01 << 16 | TOperator::TYPE_ID;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TEndIndex() : TOperator("]", getTypeId())
        {
        }
        Token* clone() const override
        {
            return new TEndIndex;
        }
    };

    struct TValueBool : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 17;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TValueBool(bool v) : Token(getTypeId()), value(v)
        {
        }
        std::string toString() const override
        {
            return value ? "true" : "false";
        }
        Token* clone() const override
        {
            return new TValueBool(value);
        }
        bool getValue() const
        {
            return value;
        }

        bool value;
    };
    struct TValueIdentifier : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 18;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TValueIdentifier(const StringId& v) : Token(getTypeId()), value(v)
        {
        }
        std::string toString() const override
        {
            return value.toString();
        }
        Token* clone() const override
        {
            return new TValueIdentifier(value);
        }
        const StringId& getValue() const
        {
            return value;
        }

        StringId value;
    };
    struct TValueNumber : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 19;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TValueNumber(double v) : Token(getTypeId()), value(v)
        {
        }
        std::string toString() const override
        {
            return "Number";
        }
        Token* clone() const override
        {
            return new TValueNumber(value);
        }
        double getValue() const
        {
            return value;
        }

        double value;
    };
    struct TValueString : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 20;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TValueString(const std::string& v) : Token(getTypeId()), value(v)
        {
        }
        std::string toString() const override
        {
            return value;
        }
        Token* clone() const override
        {
            return new TValueString(value);
        }
        const std::string& getValue() const
        {
            return value;
        }
        void setString(const std::string& s)
        {
            value = s;
        }

        std::string value;
    };
    struct TValueVoid : public Token
    {
        static const uint32_t TYPE_ID = 0x01 << 21;
        static uint32_t getTypeId()
        {
            return TYPE_ID;
        }
        TValueVoid() : Token(getTypeId())
        {
        }
        std::string toString() const override
        {
            return "void";
        }
        Token* clone() const override
        {
            return new TValueVoid;
        }
    };

    class Exception : public ExtObject
    {
        ES_PROVIDES_TYPE_NAME(Exception)
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);
        // ----

        explicit Exception(const std::string& msg, int line = 0, Type* type = nullptr);
        virtual ~Exception()
        {
        }

        void setMessage(const std::string& newMessage)
        {
            msg = newMessage;
        }
        const std::string& getMessage() const
        {
            return msg;
        }

        int getLine() const
        {
            return line;
        }
        void setLine(int newLine)
        {
            line = newLine;
        }

        const std::string& getStackInfo() const
        {
            return stackInfo;
        }
        void setStackInfo(const std::string& s)
        {
            stackInfo = s;
        }

        void setFilename(const std::string& filename)
        {
            filenameId = filename;
        }
        void setFilenameId(StringId _filenameId)
        {
            filenameId = _filenameId;
        }
        std::string getFilename() const
        {
            return filenameId.toString();
        }
        StringId getFilenameId() const
        {
            return filenameId;
        }

        //! ---|> [Object]
        Object* clone() const override;
        std::string toString() const override;

    protected:
        std::string msg;
        std::string stackInfo;
        int line;
        StringId filenameId;
    };


    //! [Tokenizer]
    class Tokenizer
    {
    public:
        typedef std::unordered_map<StringId, _CountedRef<Token>> tokenMap_t;
        typedef std::vector<_CountedRef<Token>> tokenList_t;
        static Token* identifyStaticToken(StringId id);

        //!	[Tokenizer::Error] ---|> [Exception] ---|> [Object]
        class Error : public Exception
        {
        public:
            explicit Error(const std::string& s, int _line = -1) : Exception(std::string("Tokenizer: ") + s)
            {
                setLine(_line);
            }
        };

        void getTokens(const std::string& codeU8, tokenList_t& tokens);
        void defineToken(const std::string& name, Token* value);

    private:
        Token* readNextToken(const std::string& codeU8, std::size_t& cursor, int& line, size_t& startPos, tokenList_t& tokens);
        Token* identifyToken(StringId id) const;

        static bool isNumber(const char c)
        {
            return c >= '0' && c <= '9';
        }
        static bool isChar(char c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c < 0;
        }
        static bool isWhitechar(char c)
        {
            return (c == '\n' || c == ' ' || c == '\t' || c == 13 || c == 3);
        }
        static bool isOperator(char c)
        {
            return strchr("+-/*|%&!<>=^.?:~@", c) != nullptr;
        }

        tokenMap_t customTokens;
    };



    //! [Parser]
    class Parser
    {
    public:
        Parser(Logger* logger = nullptr);
        ERef<AST::Block> parse(const CodeFragment& code);

    private:
        _CountedRef<Logger> logger;
    };



    /*! Collection of "things" used during the compilation process of one user function function
	(or block of code without surrounding function).*/
    class FnCompileContext
    {
        Compiler& compiler;
        StaticData& staticData;
        InstructionBlock& instructions;

        typedef std::unordered_map<StringId, varLocation_t> varLocationMap_t;

    public:
        enum setting_t
        {
            VISIBLE_LOCAL_AND_STATIC_VARIABLES,//!< the local variables declared in a Block
            BREAK_MARKER,
            CONTINUE_MARKER,
            EXCEPTION_MARKER//!< the marker of the next variables declared in a Block
        };

    private:
        struct SettingsStackEntry
        {
            setting_t type;
            uint32_t marker;
            varLocationMap_t declaredVariables;

            SettingsStackEntry(setting_t _type = VISIBLE_LOCAL_AND_STATIC_VARIABLES)
            : type(_type), marker(Instruction::INVALID_JUMP_ADDRESS)
            {
            }
            SettingsStackEntry(setting_t _type, uint32_t _marker) : type(_type), marker(_marker)
            {
            }
        };

        std::vector<SettingsStackEntry> settingsStack;

        //! Local variable collections needed for determining which variables have been declared inside a 'try'-block
        std::stack<std::vector<size_t>*> variableCollectorStack;

        int currentLine;
        uint32_t currentMarkerId;
        uint32_t currentOnceMarkerCounter;// used for @(once) [statement]

        CodeFragment code;
        FnCompileContext* parent;// used for detecting the visibility of static variables
        bool usesStaticVars;// if true, the function has to reference the static data container
    public:
        FnCompileContext(Compiler& _compiler, StaticData& sData, InstructionBlock& _instructions, const CodeFragment& _code)
        : compiler(_compiler), staticData(sData), instructions(_instructions), currentLine(-1),
          currentMarkerId(Instruction::JMP_TO_MARKER_OFFSET), currentOnceMarkerCounter(0), code(_code), parent(nullptr),
          usesStaticVars(false)
        {
        }

        // create a context for a function embedded another function
        FnCompileContext(FnCompileContext& parentCtxt, InstructionBlock& _instructions, const CodeFragment& _code)
        : compiler(parentCtxt.compiler), staticData(parentCtxt.staticData), instructions(_instructions),
          currentLine(-1), currentMarkerId(Instruction::JMP_TO_MARKER_OFFSET), currentOnceMarkerCounter(0), code(_code),
          parent(&parentCtxt), usesStaticVars(false)
        {
        }

        void addInstruction(const Instruction& newInstruction)
        {
            instructions.addInstruction(newInstruction, currentLine);
        }

        //! Collect all variable indices on the settings stack until an entry with the given type is found.
        std::vector<size_t> collectLocalVariables(setting_t entryType);

        void addExpression(EPtr<AST::ASTNode> expression);
        void addStatement(EPtr<AST::ASTNode> stmt);

        uint32_t createMarker()
        {
            return currentMarkerId++;
        }
        StringId createOnceStatementMarker();// used for @(once) [statement]
        uint32_t declareString(const std::string& str)
        {
            return instructions.declareString(str);
        }

        const CodeFragment& getCode() const
        {
            return code;
        }
        Compiler& getCompiler() const
        {
            return compiler;
        }
        StaticData& getStaticData() const
        {
            return staticData;
        }
        int getCurrentLine() const
        {
            return currentLine;
        }
        //! if the setting is not defined, Instruction::INVALID_JUMP_ADDRESS is returned.
        uint32_t getCurrentMarker(setting_t markerType) const;

        varLocation_t getCurrentVarLocation(const StringId& name) const;

        std::string getInstructionsAsString() const
        {
            return instructions.toString();
        }
        StringId getLocalVarName(const size_t index) const
        {
            return instructions.getLocalVarName(index);
        }

        size_t getNumLocalVars() const
        {
            return instructions.getNumLocalVars();
        }
        std::string getStringConstant(const uint32_t index) const
        {
            return instructions.getStringConstant(index);
        }
        bool getUsesStaticVars() const
        {
            return usesStaticVars;
        }

        void markAsUsingStaticVars()
        {
            usesStaticVars = true;
        }

        //! Add the local variables which are already defined in the instructionBlock (e.g. 'this' or the parameters), to the set of visible variables.
        void pushSetting_basicLocalVars();

        void pushSetting_marker(setting_t type, const uint32_t marker)
        {
            settingsStack.push_back(SettingsStackEntry(type, marker));
        }

        void pushSetting_declaredVars(const declaredVariableMap_t& variables);
        void popSetting()
        {
            settingsStack.pop_back();
        }

        uint32_t registerInternalFunction(const ObjPtr& userFunction)
        {
            return instructions.registerInternalFunction(userFunction);
        }
        void setLine(int l)
        {
            currentLine = l;
        }

        //! All newly defined variables are also added to the topmost collection.
        void pushLocalVarsCollector(std::vector<size_t>* collection)
        {
            variableCollectorStack.push(collection);
        }
        void popLocalVarsCollector()
        {
            variableCollectorStack.pop();
        }
    };

    //! Common identifiers and other numerical constants
    struct Consts
    {
        static const StringId FILENAME_INLINE;

        static const StringId IDENTIFIER_attr_printableName;
        static const StringId IDENTIFIER_this;
        static const StringId IDENTIFIER_thisFn;
        static const StringId IDENTIFIER_internalResult;
        static const StringId IDENTIFIER_fn_call;
        static const StringId IDENTIFIER_fn_checkConstraint;
        static const StringId IDENTIFIER_fn_constructor;
        static const StringId IDENTIFIER_fn_greater;
        static const StringId IDENTIFIER_fn_it_end;
        static const StringId IDENTIFIER_fn_it_next;
        static const StringId IDENTIFIER_fn_it_key;
        static const StringId IDENTIFIER_fn_it_value;
        static const StringId IDENTIFIER_fn_less;
        static const StringId IDENTIFIER_fn_equal;
        static const StringId IDENTIFIER_fn_identical;
        static const StringId IDENTIFIER_fn_getIterator;
        static const StringId IDENTIFIER_fn_get;
        static const StringId IDENTIFIER_fn_set;

        static const StringId IDENTIFIER_true;
        static const StringId IDENTIFIER_false;
        static const StringId IDENTIFIER_void;
        static const StringId IDENTIFIER_null;

        static const StringId IDENTIFIER_as;
        static const StringId IDENTIFIER_break;
        static const StringId IDENTIFIER_catch;
        static const StringId IDENTIFIER_case;
        static const StringId IDENTIFIER_continue;
        static const StringId IDENTIFIER_default;
        static const StringId IDENTIFIER_do;
        static const StringId IDENTIFIER_else;
        static const StringId IDENTIFIER_exit;
        static const StringId IDENTIFIER_for;
        static const StringId IDENTIFIER_foreach;
        static const StringId IDENTIFIER_if;
        static const StringId IDENTIFIER_namespace;
        static const StringId IDENTIFIER_return;
        static const StringId IDENTIFIER_static;
        static const StringId IDENTIFIER_switch;
        static const StringId IDENTIFIER_throw;
        static const StringId IDENTIFIER_try;
        static const StringId IDENTIFIER_var;
        static const StringId IDENTIFIER_while;
        static const StringId IDENTIFIER_yield;

        static const StringId IDENTIFIER_LINE;

        static const StringId ANNOTATION_ATTR_const;
        static const StringId ANNOTATION_ATTR_init;
        static const StringId ANNOTATION_ATTR_member;
        static const StringId ANNOTATION_ATTR_override;
        static const StringId ANNOTATION_ATTR_private;
        static const StringId ANNOTATION_ATTR_public;
        static const StringId ANNOTATION_ATTR_type;

        static const StringId ANNOTATION_FN_super;

        static const StringId ANNOTATION_STMT_once;

        static const size_t LOCAL_VAR_INDEX_this = 0;// $0
        static const size_t LOCAL_VAR_INDEX_thisFn = 1;// $1
        static const size_t LOCAL_VAR_INDEX_internalResult = 2;// $2
        static const size_t LOCAL_VAR_INDEX_firstParameter = 3;// $3

        static const size_t SYS_CALL_CREATE_ARRAY = 0;
        static const size_t SYS_CALL_CREATE_MAP = 1;
        static const size_t SYS_CALL_THROW_TYPE_EXCEPTION = 2;
        static const size_t SYS_CALL_THROW = 3;
        static const size_t SYS_CALL_EXIT = 4;
        static const size_t SYS_CALL_GET_ITERATOR = 5;
        static const size_t SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS = 6;
        static const size_t SYS_CALL_EXPAND_PARAMS_ON_STACK = 7;
        static const size_t SYS_CALL_CASE_TEST = 8;
        static const size_t SYS_CALL_ONCE = 9;
        static const size_t SYS_CALL_GET_STATIC_VAR = 10;
        static const size_t SYS_CALL_SET_STATIC_VAR = 11;
        static const size_t NUM_SYS_CALLS = 12;

        static const uint32_t DYNAMIC_PARAMETER_COUNT = ~0u;
    };

    namespace AST
    {
        //! [AnnotatedStatement]  ---|> [ASTNode]
        class AnnotatedStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(AnnotatedStatement)
        public:
            AnnotatedStatement(ptr_t _statement, const StringId& _annotationName)
            : ASTNode(TYPE_ANNOTATED_STATEMENT, false), statement(_statement), annotationName(_annotationName)
            {
            }
            virtual ~AnnotatedStatement()
            {
            }

            StringId getName() const
            {
                return annotationName;
            }
            ptr_t getStatement() const
            {
                return statement.get();
            }

        private:
            ref_t statement;
            StringId annotationName;
            // add annotation parameters when needed
        };

        //! [ConditionalExpr]  ---|> [ASTNode]
        class ConditionalExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(ConditionalExpr)
        public:
            explicit ConditionalExpr(ptr_t _condition = nullptr, ptr_t _action = nullptr, ptr_t _elseAction = nullptr)
            : ASTNode(TYPE_CONDITIONAL_EXPRESSION, true), condition(_condition), action(_action), elseAction(_elseAction)
            {
            }
            virtual ~ConditionalExpr()
            {
            }

            ptr_t getCondition() const
            {
                return condition;
            }
            ptr_t getAction() const
            {
                return action;
            }
            ptr_t getElseAction() const
            {
                return elseAction;
            }

        private:
            ref_t condition;
            ref_t action;
            ref_t elseAction;
        };

        //! [BreakStatement]  ---|> [ASTNode]
        class BreakStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(BreakStatement)
        public:
            BreakStatement() : ASTNode(TYPE_BREAK_STATEMENT, false)
            {
            }
            virtual ~BreakStatement()
            {
            }
        };

        //! [ContinueStatement]  ---|> [ASTNode]
        class ContinueStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(ContinueStatement)
        public:
            ContinueStatement() : ASTNode(TYPE_CONTINUE_STATEMENT, false)
            {
            }
            virtual ~ContinueStatement()
            {
            }
        };

        //! [ExitStatement]  ---|> [ASTNode]
        class ExitStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(ExitStatement)
        public:
            ExitStatement(ptr_t _valueExpr) : ASTNode(TYPE_EXIT_STATEMENT, false), valueExpr(_valueExpr)
            {
            }
            virtual ~ExitStatement()
            {
            }
            ptr_t getValueExpression()
            {
                return valueExpr;
            }

        private:
            ref_t valueExpr;
        };

        //! [ReturnStatement]  ---|> [ASTNode]
        class ReturnStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(ReturnStatement)
        public:
            ReturnStatement(ptr_t _valueExpr) : ASTNode(TYPE_RETURN_STATEMENT, false), valueExpr(_valueExpr)
            {
            }
            virtual ~ReturnStatement()
            {
            }
            ptr_t getValueExpression()
            {
                return valueExpr;
            }

        private:
            ref_t valueExpr;
        };
        //! [ThrowStatement]  ---|> [ASTNode]
        class ThrowStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(ThrowStatement)
        public:
            ThrowStatement(ptr_t _valueExpr) : ASTNode(TYPE_THROW_STATEMENT, false), valueExpr(_valueExpr)
            {
            }
            virtual ~ThrowStatement()
            {
            }
            ptr_t getValueExpression()
            {
                return valueExpr;
            }

        private:
            ref_t valueExpr;
        };

        //! [YieldStatement]  ---|> [ASTNode]
        class YieldStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(YieldStatement)
        public:
            YieldStatement(ptr_t _valueExpr) : ASTNode(TYPE_YIELD_STATEMENT, false), valueExpr(_valueExpr)
            {
            }
            virtual ~YieldStatement()
            {
            }
            ptr_t getValueExpression()
            {
                return valueExpr;
            }

        private:
            ref_t valueExpr;
        };


        //! [FunctionCallExpr]  ---|> [ASTNode]
        class FunctionCallExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(FunctionCallExpr)
        public:
            static FunctionCallExpr* createConstructorCall(ptr_t objExpr, const refArray_t& parameterExpr, int line = -1)
            {
                return new FunctionCallExpr(objExpr, parameterExpr, true, line);
            }

            static FunctionCallExpr* createFunctionCall(ptr_t getFunctionExpr, const refArray_t& parameterExpr, int line = -1)
            {
                return new FunctionCallExpr(getFunctionExpr, parameterExpr, false, line);
            }

            static FunctionCallExpr* createSysCall(uint32_t sysCallId, const refArray_t& parameterExpr, int line = -1)
            {
                return new FunctionCallExpr(sysCallId, parameterExpr, line);
            }
            virtual ~FunctionCallExpr()
            {
            }

            //! only valid if constructorCall == false and sysCall == false
            ptr_t getGetFunctionExpression() const
            {
                return expRef;
            }

            //! only valid if constructorCall == true
            ptr_t getObjectExpression() const
            {
                return expRef;
            }

            //! only valid if sysCall == true
            uint32_t getSysCallId() const
            {
                return sysCallId;
            }

            bool isConstructorCall() const
            {
                return constructorCall;
            }
            bool isSysCall() const
            {
                return sysCall;
            }
            const refArray_t& getParams() const
            {
                return parameters;
            }
            size_t getNumParams() const
            {
                return parameters.size();
            }
            ptr_t getParamExpression(size_t i) const
            {
                return parameters[i].get();
            }

            const std::vector<uint32_t>& getExpandingParameters() const
            {
                return expandingParameters;
            }
            bool hasExpandingParameters() const
            {
                return !expandingParameters.empty();
            }
            void emplaceExpandingParameters(std::vector<uint32_t>&& v)
            {
                expandingParameters = v;
            }

        protected:
            FunctionCallExpr(ptr_t exp, const refArray_t& _parameters, bool _isConstructorCall, int _line)
            : ASTNode(TYPE_FUNCTION_CALL_EXPRESSION, true, _line), expRef(exp), parameters(_parameters),
              constructorCall(_isConstructorCall), sysCall(false), sysCallId(0)
            {
            }

            FunctionCallExpr(uint32_t _sysCallId, const refArray_t& _parameters, int _line)
            : ASTNode(TYPE_FUNCTION_CALL_EXPRESSION, true, _line), expRef(nullptr), parameters(_parameters),
              constructorCall(false), sysCall(true), sysCallId(_sysCallId)
            {
            }

            ref_t expRef;
            refArray_t parameters;
            bool constructorCall;
            bool sysCall;
            uint32_t sysCallId;
            std::vector<uint32_t> expandingParameters;
        };

        //! [GetAttributeExpr]  ---|> [ASTNode]
        class GetAttributeExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(GetAttributeExpr)
        public:
            GetAttributeExpr(ptr_t _obj, StringId _attrId)
            : ASTNode(TYPE_GET_ATTRIBUTE_EXPRESSION, true), objExpression(_obj), attrId(_attrId)
            {
            }
            virtual ~GetAttributeExpr()
            {
            }

            const StringId& getAttrId() const
            {
                return attrId;
            }
            const std::string& getAttrName() const
            {
                return attrId.toString();
            }
            ptr_t getObjectExpression() const
            {
                return objExpression;
            }

        private:
            ref_t objExpression;
            StringId attrId;
        };

        //! [IfStatement]  ---|> [ASTNode]
        class IfStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(IfStatement)
        public:
            IfStatement(ptr_t _condition, ptr_t _action, ptr_t _elseAction)
            : ASTNode(TYPE_IF_STATEMENT, false), condition(_condition), actionStatement(_action), elseActionStatement(_elseAction)
            {
            }
            virtual ~IfStatement()
            {
            }

            ptr_t getCondition() const
            {
                return condition;
            }
            ptr_t getAction() const
            {
                return actionStatement;
            }
            ptr_t getElseAction() const
            {
                return elseActionStatement;
            }

        private:
            ref_t condition;
            ref_t actionStatement;
            ref_t elseActionStatement;
        };

        //! [LogicOpExpr]  ---|> [ASTNode]
        class LogicOpExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(LogicOpExpr)
        public:
            enum opType_T
            {
                OR,
                AND,
                NOT
            };

            static LogicOpExpr* createAnd(ptr_t _left, ptr_t _right)
            {
                return new LogicOpExpr(_left, _right, LogicOpExpr::AND);
            }
            static LogicOpExpr* createNot(ptr_t expr)
            {
                return new LogicOpExpr(expr, nullptr, LogicOpExpr::NOT);
            }
            static LogicOpExpr* createOr(ptr_t _left, ptr_t _right)
            {
                return new LogicOpExpr(_left, _right, LogicOpExpr::OR);
            }

            virtual ~LogicOpExpr()
            {
            }

            ptr_t getLeft() const
            {
                return left;
            }
            ptr_t getRight() const
            {
                return right;
            }
            opType_T getOperator() const
            {
                return op;
            }

        private:
            LogicOpExpr(ptr_t _left, ptr_t _right, opType_T _op)
            : ASTNode(TYPE_LOGIC_OP_EXPRESSION, true), left(_left), right(_right), op(_op)
            {
            }

            ref_t left, right;
            opType_T op;
        };

        //! [LoopStatement]  ---|> [ASTNode]
        class LoopStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(LoopStatement)
        public:
            //! (static) Factory: for( @p initStmt; @p cond; @p increaseStmt) @p action
            static LoopStatement* createForLoop(ptr_t _initStmt, ptr_t cond, ptr_t increaseStmt, ptr_t action, ptr_t _elseAction = nullptr)
            {
                return new LoopStatement(_initStmt, cond, action, nullptr, increaseStmt, _elseAction);
            }

            //! (static) Factory: while( @p cond ) @p action
            static LoopStatement* createWhileLoop(ptr_t cond, ptr_t _action, ptr_t _elseAction = nullptr)
            {
                return new LoopStatement(nullptr, cond, _action, nullptr, nullptr, _elseAction);
            }

            //! (static) Factory: do @p action while( @p cond )
            static LoopStatement* createDoWhileLoop(ptr_t cond, ptr_t _action, ptr_t _elseAction = nullptr)
            {
                return new LoopStatement(nullptr, nullptr, _action, cond, nullptr, _elseAction);
            }

            virtual ~LoopStatement()
            {
            }

            ptr_t getInitStatement() const
            {
                return initStmt;
            }
            ptr_t getPreConditionExpression() const
            {
                return preConditionExpression;
            }
            ptr_t getAction() const
            {
                return actionStmt;
            }
            ptr_t getPostConditionExpression() const
            {
                return postConditionExpression;
            }
            ptr_t getIncreaseStatement() const
            {
                return increaseStmt;
            }
            ptr_t getElseAction() const
            {
                return elseAction;
            }

        private:
            LoopStatement(ptr_t _initStmt, ptr_t _preConditionExpression, ptr_t _action, ptr_t _postConditionExpression, ptr_t _increaseStmt, ptr_t _elseAction)
            : ASTNode(TYPE_LOOP_STATEMENT, false), initStmt(_initStmt), preConditionExpression(_preConditionExpression),
              actionStmt(_action), postConditionExpression(_postConditionExpression), increaseStmt(_increaseStmt),
              elseAction(_elseAction)
            {
            }

            ref_t initStmt;
            ref_t preConditionExpression;
            ref_t actionStmt;
            ref_t postConditionExpression;
            ref_t increaseStmt;
            ref_t elseAction;
        };

        //! [SetAttributeExpr]  ---|> [ASTNode]
        class SetAttributeExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(SetAttributeExpr)
        public:
            static SetAttributeExpr* createAssignment(ptr_t obj, StringId attrId, ptr_t valueExp, int _line = -1)
            {
                SetAttributeExpr* sa = new SetAttributeExpr(obj, attrId, valueExp, 0, _line);
                sa->assign = true;
                return sa;
            }

            SetAttributeExpr(ptr_t obj, StringId _attrId, ptr_t _valueExp, Attribute::flag_t _attrFlags, int _line = -1)
            : ASTNode(TYPE_SET_ATTRIBUTE_EXPRESSION, _line), objExpr(obj), valueExpr(_valueExp), attrId(_attrId),
              attrFlags(_attrFlags), assign(false)
            {
            }

            virtual ~SetAttributeExpr()
            {
            }

            const StringId& getAttrId() const
            {
                return attrId;
            }
            ptr_t getObjectExpression() const
            {
                return objExpr;
            }
            Attribute::flag_t getAttributeProperties() const
            {
                return attrFlags;
            }
            ptr_t getValueExpression() const
            {
                return valueExpr;
            }
            std::string getAttrName() const
            {
                return attrId.toString();
            }
            bool isAssignment() const
            {
                return assign;
            }

        private:
            friend class EScript::Runtime;
            ref_t objExpr;
            ref_t valueExpr;
            StringId attrId;
            Attribute::flag_t attrFlags;
            bool assign;
        };

        //! [SwitchCaseStatement]  ---|> [ASTNode]
        class SwitchCaseStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(SwitchCaseStatement)
        public:
            //! [ (statementIndex,conditionExpr)* ]
            typedef std::vector<std::pair<size_t, ERef<AST::ASTNode>>> caseInfoVector_t;
            SwitchCaseStatement(ptr_t _decisionExpr, Block* _block, caseInfoVector_t&& _caseInfos)
            : ASTNode(TYPE_SWITCH_STATEMENT, false), block(_block), decisionExpr(_decisionExpr), caseInfos(_caseInfos)
            {
            }
            virtual ~SwitchCaseStatement()
            {
            }

            ptr_t getDecisionExpression() const
            {
                return decisionExpr;
            }
            Block* getBlock() const
            {
                return block.get();
            }
            const caseInfoVector_t& getCaseInfos() const
            {
                return caseInfos;
            }

        private:
            ERef<Block> block;
            ref_t decisionExpr;
            caseInfoVector_t caseInfos;
        };

        //! [TryCatchStatement]  ---|> [ASTNode]
        class TryCatchStatement : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(TryCatchStatement)
        public:
            TryCatchStatement(ptr_t _tryBlock, ptr_t _catchBlock, StringId _exceptionVariableName)
            : ASTNode(TYPE_TRY_CATCH_STATEMENT, false), tryBlock(_tryBlock), catchBlock(_catchBlock),
              exceptionVariableName(_exceptionVariableName)
            {
            }
            virtual ~TryCatchStatement()
            {
            }

            const StringId& getExceptionVariableName() const
            {
                return exceptionVariableName;
            }
            ptr_t getTryBlock() const
            {
                return tryBlock;
            }
            ptr_t getCatchBlock() const
            {
                return catchBlock;
            }

        private:
            ref_t tryBlock;
            ref_t catchBlock;
            StringId exceptionVariableName;
        };



        //! [UserFunctionExpr]  ---|> [ExtObject]
        class UserFunctionExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(UserFunctionExpr)
        public:
            // -------------------------------------------------------------

            //! @name Parameter
            //	@{

            //! [Parameter]
            class Parameter
            {
            private:
                StringId name;
                ref_t defaultValueExpressionRef;
                refArray_t typeExpressions;
                bool multiParam;

            public:
                Parameter(const StringId& name, ptr_t defaultValueExpression, refArray_t&& _typeExpressions);
                StringId getName() const
                {
                    return name;
                }
                const refArray_t& getTypeExpressions() const
                {
                    return typeExpressions;
                }

                void setMultiParam(bool b)
                {
                    multiParam = b;
                }
                bool isMultiParam() const
                {
                    return multiParam;
                }

                ptr_t getDefaultValueExpression() const
                {
                    return defaultValueExpressionRef;
                }
                void setDefaultValueExpression(ptr_t newDefaultExpression)
                {
                    defaultValueExpressionRef = newDefaultExpression;
                }
            };

            typedef std::vector<Parameter> parameterList_t;

            UserFunctionExpr(Block* block, const refArray_t& _sConstrExpressions, int line);
            virtual ~UserFunctionExpr()
            {
            }

            Block* getBlock() const
            {
                return blockRef.get();
            }
            const CodeFragment& getCode() const
            {
                return code;
            }

            const parameterList_t& getParamList() const
            {
                return params;
            }
            void emplaceParameterExpressions(parameterList_t&& _params)
            {
                params = std::move(_params);
            }

            refArray_t& getSConstructorExpressions()
            {
                return sConstrExpressions;
            }
            const refArray_t& getSConstructorExpressions() const
            {
                return sConstrExpressions;
            }

            void setCode(const CodeFragment& _code)
            {
                code = _code;
            }

        private:
            ERef<Block> blockRef;
            parameterList_t params;
            refArray_t sConstrExpressions;
            CodeFragment code;
        };

        //! [BoolValueExpr]  ---|> [ASTNode]
        class BoolValueExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(BoolValueExpr)
        public:
            BoolValueExpr(bool _value, int _line = -1) : ASTNode(TYPE_VALUE_BOOL, _line), value(_value)
            {
            }
            virtual ~BoolValueExpr()
            {
            }
            bool getValue() const
            {
                return value;
            }

        private:
            bool value;
        };
        //! [IdentifierValueExpr]  ---|> [ASTNode]
        class IdentifierValueExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(IdentifierValueExpr)
        public:
            IdentifierValueExpr(const StringId& _value, int _line = -1)
            : ASTNode(TYPE_VALUE_IDENTIFIER, _line), value(_value)
            {
            }
            virtual ~IdentifierValueExpr()
            {
            }
            const StringId& getValue() const
            {
                return value;
            }

        private:
            StringId value;
        };
        //! [NumberValueExpr]  ---|> [ASTNode]
        class NumberValueExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(NumberValueExpr)
        public:
            NumberValueExpr(double _value, int _line = -1) : ASTNode(TYPE_VALUE_FLOATING_POINT, _line), value(_value)
            {
            }
            virtual ~NumberValueExpr()
            {
            }
            double getValue() const
            {
                return value;
            }
            void setValue(const double& v)
            {
                value = v;
            }

        private:
            double value;
        };
        //! [StringValueExpr]  ---|> [ASTNode]
        class StringValueExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(StringValueExpr)
        public:
            StringValueExpr(const std::string& _value, int _line = -1)
            : ASTNode(TYPE_VALUE_STRING, _line), value(_value)
            {
            }
            virtual ~StringValueExpr()
            {
            }
            const std::string& getValue() const
            {
                return value;
            }

        private:
            std::string value;
        };
        //! [VoidValueExpr]  ---|> [ASTNode]
        class VoidValueExpr : public ASTNode
        {
            ES_PROVIDES_TYPE_NAME(VoidValueExpr)
        public:
            VoidValueExpr(int _line = -1) : ASTNode(TYPE_VALUE_VOID, _line)
            {
            }
            virtual ~VoidValueExpr()
            {
            }
        };
    }

    //! Container for static variables shared among several UserFunctions.
    class StaticData : public EReferenceCounter<StaticData>
    {
        std::vector<StringId> staticVariableNames;// currently unused!
        std::vector<ObjRef> staticVariableValues;

    public:
        uint32_t declareStaticVariable(const StringId& name)
        {
            staticVariableNames.emplace_back(name);
            staticVariableValues.emplace_back(nullptr);
            return static_cast<uint32_t>(staticVariableNames.size() - 1);
        }
        const std::vector<StringId>& getStaticVariableNames() const
        {
            return staticVariableNames;
        }
        const std::vector<ObjRef>& getStaticVariableValues() const
        {
            return staticVariableValues;
        }
        bool updateStaticVariable(uint32_t index, Object* value)
        {
            if(index >= staticVariableValues.size())
                return false;
            staticVariableValues[index] = value;
            return true;
        }
    };

    //! [UserFunction]  ---|> [ExtObject]
    class UserFunction : public ExtObject
    {
        ES_PROVIDES_TYPE_NAME(UserFunction)

    public:
        static Type* getTypeObject();
        static void init(Namespace& globals);

    protected:
        UserFunction(const UserFunction& other);

    public:
        UserFunction();
        virtual ~UserFunction()
        {
        }

        const CodeFragment& getCode() const
        {
            return codeFragment;
        }
        void setCode(const CodeFragment& c)
        {
            codeFragment = c;
        }

        int getMaxParamCount() const
        {
            return maxParamValueCount;
        }
        int getMinParamCount() const
        {
            return minParamValueCount;
        }
        size_t getParamCount() const
        {
            return paramCount;
        }

        void setParameterCounts(size_t paramsCount, int minValues, int maxValues)
        {
            paramCount = paramsCount, minParamValueCount = minValues, maxParamValueCount = maxValues;
        }
        const InstructionBlock& getInstructionBlock() const
        {
            return instructions;
        }
        InstructionBlock& getInstructionBlock()
        {
            return instructions;
        }
        int getLine() const
        {
            return line;
        }
        void setLine(const int l)
        {
            line = l;
        }

        //! if multiParam >= paramCount, the additional parameter values are to be ignored. e.g. fn(a,...)
        int getMultiParam() const
        {
            return multiParam;
        }
        void setMultiParam(int i)
        {
            multiParam = i;
        }

        StaticData* getStaticData() const
        {
            return staticData.get();
        }
        void setStaticData(_CountedRef<StaticData>&& d)
        {
            staticData = d;
        }

        //! ---|> [Object]
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_USER_FUNCTION;
        }
        UserFunction* clone() const override
        {
            return new UserFunction(*this);
        }
        std::string toDbgString() const override;

    private:
        CodeFragment codeFragment;
        int line;
        size_t paramCount;
        int minParamValueCount, maxParamValueCount, multiParam;

        InstructionBlock instructions;
        _CountedRef<StaticData> staticData;
    };

    //! [FunctionCallContext]
    class FunctionCallContext : public EReferenceCounter<FunctionCallContext, FunctionCallContext>
    {
    private:
        static std::stack<FunctionCallContext*> pool;

    public:
        static FunctionCallContext* create(const EPtr<UserFunction> userFunction, const ObjPtr _caller);
        static void release(FunctionCallContext* rts);
    public:
        typedef std::vector<Instruction>::const_iterator instructionCursor_t;

    private:
        enum error_t
        {
            STACK_EMPTY_ERROR,
            STACK_WRONG_DATA_TYPE,
            UNKNOWN_LOCAL_VARIABLE,
            UNKNOWN_STATIC_VARIABLE,
        };
        void throwError(error_t error) const;

        ERef<UserFunction> userFunction;
        size_t exceptionHandlerPos;
        instructionCursor_t instructionCursor;
        ObjRef caller;

        FunctionCallContext()
        : exceptionHandlerPos(0), constructorCall(false), providesCallerAsResult(false), stopExecutionAfterEnding(false)
        {
        }
        ~FunctionCallContext()
        {
        }
        void reset();
        void init(const EPtr<UserFunction> userFunction, const ObjPtr _caller);

        bool constructorCall;
        bool providesCallerAsResult;
        bool stopExecutionAfterEnding;//! ... or otherwise, continue with the execution of the parent-context.

    public:
        /*! Marks that the return value of the context should be used as the calling context's calling object.
			This is the case, if the context belongs to a superconstructor call. */
        void markAsProvidingCallerAsResult()
        {
            providesCallerAsResult = true;
        }
        void enableStopExecutionAfterEnding()
        {
            stopExecutionAfterEnding = true;
        }

        ObjPtr getCaller() const
        {
            return caller;
        }
        int getCurrentLine() const
        {
            return instructionCursor == getInstructions().end() ? -1 : instructionCursor->getLine();
        }
        size_t getExceptionHandlerPos() const
        {
            return exceptionHandlerPos;
        }
        const InstructionBlock& getInstructionBlock() const
        {
            return userFunction->getInstructionBlock();
        }
        const instructionCursor_t& getInstructionCursor() const
        {
            return instructionCursor;
        }
        const std::vector<Instruction>& getInstructions() const
        {
            return userFunction->getInstructionBlock().getInstructions();
        }
        EPtr<UserFunction> getUserFunction() const
        {
            return userFunction;
        }

        void increaseInstructionCursor()
        {
            ++instructionCursor;
        }

        //! Set the caller-object; the caller-member as well as the local-'this'-variable
        void initCaller(const ObjPtr _caller);
        bool isConstructorCall() const
        {
            return constructorCall;
        }
        bool isExecutionStoppedAfterEnding() const
        {
            return stopExecutionAfterEnding;
        }
        bool isProvidingCallerAsResult() const
        {
            return providesCallerAsResult;
        }
        void markAsConstructorCall()
        {
            constructorCall = true;
        }
        void setExceptionHandlerPos(const size_t p)
        {
            exceptionHandlerPos = p;
        }
        void setInstructionCursor(const size_t p)
        {
            const std::vector<Instruction>& instructions = getInstructions();
            instructionCursor = (p >= instructions.size() ? instructions.end() : instructions.begin() + p);
        }

    private:
        std::vector<ObjRef> localVariables;

    public:
        void assignToLocalVariable(const uint32_t index, ObjRef&& value)
        {
            if(index >= localVariables.size())
                throwError(UNKNOWN_LOCAL_VARIABLE);
            localVariables[index] = std::move(value);
        }
        void assignToLocalVariable(const uint32_t index, const ObjRef& value)
        {
            if(index >= localVariables.size())
                throwError(UNKNOWN_LOCAL_VARIABLE);
            localVariables[index] = value;
        }
        Object* getLocalVariable(const uint32_t index) const
        {
            if(index >= localVariables.size())
                throwError(UNKNOWN_LOCAL_VARIABLE);
            return localVariables[index].get();
        }
        Object* getStaticVar(const uint32_t index) const
        {
            const auto data = userFunction->getStaticData();
            if(!data || index >= data->getStaticVariableValues().size())
                throwError(UNKNOWN_STATIC_VARIABLE);
            return data->getStaticVariableValues()[index].get();
        }
        std::string getLocalVariablesAsString(const bool includeUndefined) const;
        void resetLocalVariable(const uint32_t index)
        {
            if(index >= localVariables.size())
                throwError(UNKNOWN_LOCAL_VARIABLE);
            localVariables[index] = nullptr;
        }
        StringId getLocalVariableName(const uint32_t index) const
        {
            return getInstructionBlock().getLocalVariables().at(index);
        }
        void setStaticVar(const uint32_t index, Object* value)
        {
            const auto data = userFunction->getStaticData();
            if(!data || !data->updateStaticVariable(index, value))
                throwError(UNKNOWN_STATIC_VARIABLE);
        }

    private:
        std::vector<RtValue> valueStack;

    public:
        void stack_clear();
        void stack_dup()
        {
            valueStack.emplace_back(stack_top());
        }
        bool stack_empty() const
        {
            return valueStack.empty();
        }
        void stack_pushBool(const bool value)
        {
            valueStack.emplace_back(value);
        }
        void stack_pushUndefined()
        {
            valueStack.emplace_back(RtValue());
        }
        void stack_pushFunction(const uint32_t functionIndex)
        {
            valueStack.emplace_back(userFunction->getInstructionBlock().getUserFunction(functionIndex));
        }
        void stack_pushNumber(const double& value)
        {
            valueStack.emplace_back(value);
        }
        void stack_pushUInt32(const uint32_t value)
        {
            valueStack.emplace_back(value);
        }
        void stack_pushIdentifier(const StringId& strId)
        {
            valueStack.emplace_back(strId);
        }
        void stack_pushStringIndex(const uint32_t value)
        {
            valueStack.emplace_back(RtValue::createLocalStringIndex(value));
        }
        void stack_pushObject(const ObjPtr& obj)
        {
            valueStack.emplace_back(obj.get());
        }
        void stack_pushValue(RtValue&& value)
        {
            valueStack.emplace_back(std::move(value));
        }
        void stack_pushVoid()
        {
            valueStack.emplace_back(nullptr);
        }

        size_t stack_size() const
        {
            return valueStack.size();
        }
        void stack_pop()
        {
            valueStack.pop_back();
        }
        bool stack_popBool()
        {
            const bool b = stack_top().toBool();
            valueStack.pop_back();
            return b;
        }
        StringId stack_popIdentifier()
        {
            RtValue& entry = stack_top();
            if(!entry.isIdentifier())
                throwError(STACK_WRONG_DATA_TYPE);
            const StringId id(entry._getIdentifier());
            valueStack.pop_back();
            return id;
        }
        uint32_t stack_popUInt32()
        {
            RtValue& entry = stack_top();
            if(!entry.isUint32())
                throwError(STACK_WRONG_DATA_TYPE);
            const uint32_t number(entry._getUInt32());
            valueStack.pop_back();
            return number;
        }
        double stack_popNumber()
        {
            RtValue& entry = stack_top();
            if(!entry.isNumber())
                throwError(STACK_WRONG_DATA_TYPE);
            const double number(entry._getNumber());
            valueStack.pop_back();
            return number;
        }
        uint32_t stack_popStringIndex()
        {
            RtValue& entry = stack_top();
            if(!entry.isLocalString())
                throwError(STACK_WRONG_DATA_TYPE);
            const uint32_t index(entry._getLocalStringIndex());
            valueStack.pop_back();
            return index;
        }
        ObjRef stack_popObject()
        {
            ObjRef obj;
            RtValue& top = stack_top();
            if(top.isObject())
            {// fast path
                obj._set(top._detachObject());
            }
            else
            {
                obj = std::move(rtValueToObject(top));
            }
            valueStack.pop_back();
            return obj;
        }
        ObjRef rtValueToObject(RtValue& value);

        /*! Works almost like stack_popObject(), but:
			- returns an obj->cloneOrReference() if the contained value is already an Object.
			- returns nullptr (and not Void) iff the value is undefined.	This is necessary to detect undefined parameters. */
        ObjRef stack_popObjectValue();

        RtValue stack_popValue()
        {
            const RtValue v = stack_top();
            valueStack.pop_back();
            return v;
        }

        std::string stack_toDbgString() const;

    private:
        RtValue& stack_top()
        {
            if(stack_empty())
                throwError(STACK_EMPTY_ERROR);
            return valueStack.back();
        }
    };



    //! [RuntimeInternals]
    class RuntimeInternals
    {
        Runtime& runtime;
        RuntimeInternals(RuntimeInternals& other);

    public:
        RuntimeInternals(Runtime& rt);
        ~RuntimeInternals();

        void warn(const std::string& message) const;
        void setException(const std::string& message) const
        {
            runtime.setException(message);
        }

    public:
        /*! (internal)
			Start the execution of a function. A c++ function is executed immediatly and the result is <result,nullptr>.
			A UserFunction produces a FunctionCallContext which still has to be executed. The result is then result.isFunctionCallContext() == true
			\note the @p params value may be altered by this function and should not be used afterwards!	*/
        RtValue startFunctionExecution(const ObjPtr& fun, const ObjPtr& callingObject, ParameterValues& params);

        RtValue startInstanceCreation(EPtr<Type> type, ParameterValues& params);

        ObjRef executeFunctionCallContext(_Ptr<FunctionCallContext> fcc);

        ObjPtr getCallingObject() const
        {
            return activeFCCs.empty() ? nullptr : activeFCCs.back()->getCaller();
        }
        size_t getStackSize() const
        {
            return activeFCCs.size();
        }
        size_t _getStackSizeLimit() const
        {
            return stackSizeLimit;
        }
        void _setStackSizeLimit(const size_t limit)
        {
            stackSizeLimit = limit;
        }

    private:
        std::vector<_CountedRef<FunctionCallContext>> activeFCCs;
        size_t stackSizeLimit;

        _Ptr<FunctionCallContext> getActiveFCC() const
        {
            return activeFCCs.empty() ? nullptr : activeFCCs.back();
        }

        void pushActiveFCC(const _Ptr<FunctionCallContext>& fcc)
        {
            activeFCCs.push_back(fcc);
            if(activeFCCs.size() > stackSizeLimit)
                stackSizeError();
        }
        void popActiveFCC()
        {
            activeFCCs.pop_back();
        }
        void stackSizeError();

    public:
        ObjPtr getGlobalVariable(const StringId& id);
        Namespace* getGlobals() const;

    private:
        ERef<Namespace> globals;

    public:
        int getCurrentLine() const;
        std::string getCurrentFile() const;

        std::string getStackInfo();
        std::string getLocalStackInfo();

    public:
        enum state_t
        {
            STATE_NORMAL,
            STATE_EXITING,
            STATE_EXCEPTION
        };
        bool checkNormalState() const
        {
            return state == STATE_NORMAL;
        }

        ObjRef fetchAndClearException()
        {
            if(state == STATE_EXCEPTION)
            {
                state = STATE_NORMAL;
                return std::move(resultValue);
            }
            return nullptr;
        }
        ObjRef fetchAndClearExitResult()
        {
            if(state == STATE_EXITING)
            {
                state = STATE_NORMAL;
                return std::move(resultValue);
            }
            return nullptr;
        }

        state_t getState() const
        {
            return state;
        }

        void setAddStackInfoToExceptions(bool b)
        {
            addStackIngfoToExceptions = b;
        }

        /*! Creates an exception object including current stack info and
			sets the state to STATE_EXCEPTION. Does NOT throw a C++ exception. */
        void setException(const std::string& s);

        /*! Annotates the given Exception with the current stack info and set the state
			to STATE_EXCEPTION. Does NOT throw a C++ exception. */
        void setException(Exception* e);

        /**
		 * Throws a runtime exception (a C++ Exception, not an internal one!).
		 * Should only be used inside of library-functions
		 * (otherwise, they are not handled and the program is likely to crash).
		 * In all other situations try to use setException(...)
		 */
        void throwException(const std::string& s, Object* obj = nullptr);

        void setExitState(ObjRef value)
        {
            resultValue = std::move(value);
            state = STATE_EXITING;
        }
        void setExceptionState(ObjRef value)
        {
            resultValue = std::move(value);
            state = STATE_EXCEPTION;
        }

    private:
        state_t state;
        ObjRef resultValue;
        bool addStackIngfoToExceptions;

        //! (interna) Used by the Runtime.
    private:
        typedef RtValue (*sysFunctionPtr)(RuntimeInternals&, const ParameterValues&);
        std::vector<sysFunctionPtr> systemFunctions;
        void initSystemFunctions();

    public:
        RtValue sysCall(uint32_t sysFnId, ParameterValues& params);
    };

    namespace StringUtils
    {
        double readNumber(const char* s, std::size_t& cursor, bool checkSign = false);
        std::string rTrim(const std::string& s);
        std::string lTrim(const std::string& s);
        std::string trim(const std::string& s);
        std::string replaceAll(const std::string& subject, const std::string& find, const std::string& replace, int count = -1);

        //! Escape quotes, newlines and backslashes.
        std::string escape(const std::string& s);

        //! Replace all occurrences of the rules.first with the corresponding rules.second
        std::string
        replaceMultiple(const std::string& subject, const std::vector<std::pair<std::string, std::string>>& rules, int max = -1);

        //! \note this is only a hack!
        std::string UCS2LE_to_ANSII(const std::string& str);

        //! Split the subject at the occurrence of delimiter into at most max parts.
        std::vector<std::string> split(const std::string& subject, const std::string& delimiter, int max = -1);

        //! \note the first line has index 0
        std::string getLine(const std::string& s, const int lineIndex);

        size_t countCodePoints(const std::string& str_u8);

        std::string utf32_to_utf8(const uint32_t u32);
    }

    //! [FnBinder] ---|> [Object]
    class FnBinder : public Object
    {
        ES_PROVIDES_TYPE_NAME(FnBinder)
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

        static FnBinder* create(ObjPtr object, ObjPtr function);
        static FnBinder* create(ObjPtr object, ObjPtr function, std::vector<ObjRef>&& params);
        static void release(FnBinder* b);
        virtual ~FnBinder()
        {
        }

        Object* getObject() const
        {
            return myObjectRef.get();
        }
        Object* getFunction() const
        {
            return functionRef.get();
        }
        std::vector<ObjRef> getBoundParameters() const
        {
            return boundParameters;
        }
        void setObject(ObjPtr newObject)
        {
            myObjectRef = newObject;
        }
        void setFunction(ObjPtr newFunction)
        {
            functionRef = newFunction;
        }

        //! ---|> [Object]
        FnBinder* clone() const override;
        bool rt_isEqual(Runtime& rt, const ObjPtr& o) override;
        std::string toDbgString() const override;
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_FN_BINDER;
        }

    private:
        FnBinder(ObjPtr object, ObjPtr function);

        ObjRef myObjectRef, functionRef;
        std::vector<ObjRef> boundParameters;
    };



    //! [Function] ---|> [Object]
    class Function : public Object
    {
        public:
            typedef RtValue (*functionPtr)(Runtime& runtime, ObjPtr thisEObj, const ParameterValues& parameter);


        ES_PROVIDES_TYPE_NAME(Function)

    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

    public:
        Function(functionPtr fnptr);
        Function(StringId originalName, int minParamCount, int maxParamCount, functionPtr fnptr);
        virtual ~Function()
        {
        }

        int getCallCounter() const
        {
            return callCounter;
        }
        functionPtr getFnPtr() const
        {
            return fnptr;
        }
        int getMaxParamCount() const
        {
            return maxParamCount;
        }
        int getMinParamCount() const
        {
            return minParamCount;
        }
        StringId getOriginalName() const
        {
            return originalName;
        }
        void increaseCallCounter()
        {
            ++callCounter;
        }
        void resetCallCounter()
        {
            callCounter = 0;
        }

        Object* clone() const override
        {
            return new Function(fnptr);
        }
        internalTypeId_t _getInternalTypeId() const override
        {
            return _TypeIds::TYPE_FUNCTION;
        }

    private:
        functionPtr fnptr;
        int minParamCount, maxParamCount;
        StringId originalName;
        int callCounter;
    };

    //! [YieldIterator] ---|> [Object]
    class YieldIterator : public Object
    {
        ES_PROVIDES_TYPE_NAME(YieldIterator)
    public:
        static Type* getTypeObject();
        static void init(EScript::Namespace& globals);

        YieldIterator() : Object(getTypeObject()), counter(0)
        {
        }
        virtual ~YieldIterator()
        {
        }

        Object* value() const
        {
            return currentValue.get();
        }
        Object* key() const;
        void setValue(Object* newResult)
        {
            currentValue = newResult;
        }

        _Ptr<FunctionCallContext> getFCC() const
        {
            return fcc;
        }
        void setFCC(_Ptr<FunctionCallContext> _fcc)
        {
            fcc = _fcc;
        }

        void next(Runtime& rt);
        bool end() const
        {
            return fcc.isNull();
        }

        int getCounter() const
        {
            return counter;
        }

    private:
        ObjRef currentValue;
        int counter;
        _CountedRef<FunctionCallContext> fcc;
    };

}
