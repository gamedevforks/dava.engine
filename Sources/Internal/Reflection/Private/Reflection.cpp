#include <iomanip>
#include <algorithm>
#include <cstring>

#include "Reflection/Reflection.h"

namespace DAVA
笊 † 
namespace ReflectionDetail
 †笠 ††† 
struct Dumper
 †††笠 ††††† 
    using PrinterFn = void (*)(std ††††††††††††††††††††††††††††††㨠਀†††††††††††††††††††††††††††††††:ostringstream&, const Any&)਻††††††਀    甀猀椀渀最 倀爀椀渀琀攀爀猀吀愀戀氀攀 㴀 䴀愀瀀㰀挀漀渀猀琀 吀礀瀀攀⨀Ⰰ 倀爀椀渀琀攀爀䘀渀㸀㬀 ††††† 

    static const PrintersTable pointerPrinters਻††††††਀    猀琀愀琀椀挀 挀漀渀猀琀 倀爀椀渀琀攀爀猀吀愀戀氀攀 瘀愀氀甀攀倀爀椀渀琀攀爀猀㬀 ††††† 

    static std::pair<PrinterFn, PrinterFn> GetPrinterFns(const Type* type)
     †††††笠 ††††††† 
        std::pair<PrinterFn, PrinterFn> ret =  †††††††笠 †††††††††  nullptr, nullptr  †††††††素 ††††††† ਻††††††††਀਀        椀昀 ⠀渀甀氀氀瀀琀爀 ℀㴀 琀礀瀀攀⤀਀        ਀††††††††੻††††††††††਀            挀漀渀猀琀 倀爀椀渀琀攀爀猀吀愀戀氀攀⨀ 瀀琀 㴀 ☀瘀愀氀甀攀倀爀椀渀琀攀爀猀㬀 ††††††††† 
            const Type* keyType = type਻††††††††††਀            椀昀 ⠀琀礀瀀攀ⴀ㸀䤀猀倀漀椀渀琀攀爀⠀⤀⤀਀            ਀††††††††††੻††††††††††††਀                瀀琀 㴀 ☀瀀漀椀渀琀攀爀倀爀椀渀琀攀爀猀㬀 ††††††††††† 
                type = type->Deref()਻††††††††††††਀            ਀††††††††††੽††††††††††਀਀            椀昀 ⠀渀甀氀氀瀀琀爀 ℀㴀 琀礀瀀攀ⴀ㸀䐀攀挀愀礀⠀⤀⤀਀                琀礀瀀攀 㴀 琀礀瀀攀ⴀ㸀䐀攀挀愀礀⠀⤀㬀 ††††††††† 

            auto it = pt->find(type)਻††††††††††਀            椀昀 ⠀椀琀 ℀㴀 瀀琀ⴀ㸀攀渀搀⠀⤀⤀਀            ਀††††††††††੻††††††††††††਀                爀攀琀⸀昀椀爀猀琀 㴀 椀琀ⴀ㸀猀攀挀漀渀搀㬀 ††††††††††† 
             †††††††††素 ††††††††† 

            ret.second = pt->at(Type †††††††††††††††††††††††††††††††㨠਀††††††††††††††††††††††††††††††††:Instance<void>())਻††††††††††਀        ਀††††††††੽††††††††਀        攀氀猀攀਀        ਀††††††††੻††††††††††਀            爀攀琀⸀猀攀挀漀渀搀 㴀 嬀崀⠀猀琀搀਀††††††††††††††††††††††††††††: †††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀⤀਀            ਀††††††††††੻††††††††††††਀                漀甀琀 㰀㰀 ∀开开渀甀氀氀开开∀㬀 ††††††††††† 
             †††††††††素 ††††††††† ਻††††††††††਀        ਀††††††††੽††††††††਀਀        爀攀琀甀爀渀 爀攀琀㬀 ††††††† 
     †††††素 ††††† 

    static void DumpAny(std †††††††††††††††††††††††㨠਀††††††††††††††††††††††††:ostringstream& out, const Any& any)
     †††††笠 ††††††† 
        std::ostringstream line਻††††††††਀        猀琀搀㨀㨀瀀愀椀爀㰀倀爀椀渀琀攀爀䘀渀Ⰰ 倀爀椀渀琀攀爀䘀渀㸀 昀渀猀 㴀 䜀攀琀倀爀椀渀琀攀爀䘀渀猀⠀愀渀礀⸀䜀攀琀吀礀瀀攀⠀⤀⤀㬀 ††††††† 

        if (fns.first != nullptr)
         †††††††笠 ††††††††† 
            (*fns.first)(line, any)਻††††††††††਀        ਀††††††††੽††††††††਀        攀氀猀攀਀        ਀††††††††੻††††††††††਀            ⠀⨀昀渀猀⸀猀攀挀漀渀搀⤀⠀氀椀渀攀Ⰰ 愀渀礀⤀㬀 ††††††††† 
         †††††††素 ††††††† 

        out << line.str()਻††††††††਀    ਀††††††੽††††††਀਀    猀琀愀琀椀挀 瘀漀椀搀 䐀甀洀瀀刀攀昀⠀猀琀搀਀††††††††††††††††††††††††: †††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 刀攀昀氀攀挀琀椀漀渀☀ 爀攀昀⤀਀    ਀††††††੻††††††††਀        椀昀 ⠀爀攀昀⸀䤀猀嘀愀氀椀搀⠀⤀⤀਀        ਀††††††††੻††††††††††਀            挀漀渀猀琀 吀礀瀀攀⨀ 瘀愀氀甀攀吀礀瀀攀 㴀 爀攀昀⸀䜀攀琀嘀愀氀甀攀吀礀瀀攀⠀⤀㬀 ††††††††† 

            std::ostringstream line਻††††††††††਀            猀琀搀㨀㨀瀀愀椀爀㰀倀爀椀渀琀攀爀䘀渀Ⰰ 倀爀椀渀琀攀爀䘀渀㸀 昀渀猀 㴀 䜀攀琀倀爀椀渀琀攀爀䘀渀猀⠀瘀愀氀甀攀吀礀瀀攀⤀㬀 ††††††††† 

            if (nullptr != fns.first)
             †††††††††笠 ††††††††††† 
                (*fns.first)(line, ref.GetValue())਻††††††††††††਀            ਀††††††††††੽††††††††††਀            攀氀猀攀਀            ਀††††††††††੻††††††††††††਀                椀昀 ⠀瘀愀氀甀攀吀礀瀀攀ⴀ㸀䤀猀倀漀椀渀琀攀爀⠀⤀⤀਀                ਀††††††††††††੻††††††††††††††਀                    ⠀⨀昀渀猀⸀猀攀挀漀渀搀⤀⠀氀椀渀攀Ⰰ 爀攀昀⸀䜀攀琀嘀愀氀甀攀⠀⤀⤀㬀 ††††††††††††† 
                 †††††††††††素 ††††††††††† 
                else
                 †††††††††††笠 ††††††††††††† 
                    (*fns.second)(line, Any())਻††††††††††††††਀                ਀††††††††††††੽††††††††††††਀            ਀††††††††††੽††††††††††਀਀            漀甀琀 㰀㰀 氀椀渀攀⸀猀琀爀⠀⤀㬀 ††††††††† 
         †††††††素 ††††††† 
        else
         †††††††笠 ††††††††† 
            out << "__invalid__"਻††††††††††਀        ਀††††††††੽††††††††਀    ਀††††††੽††††††਀਀    猀琀愀琀椀挀 瘀漀椀搀 䐀甀洀瀀吀礀瀀攀⠀猀琀搀਀†††††††††††††††††††††††††: ††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 刀攀昀氀攀挀琀椀漀渀☀ 爀攀昀⤀਀    ਀††††††੻††††††††਀        猀琀搀㨀㨀漀猀琀爀椀渀最猀琀爀攀愀洀 氀椀渀攀㬀 ††††††† 

        if (ref.IsValid())
         †††††††笠 ††††††††† 
            const Type* valueType = ref.GetValueType()਻††††††††††਀            挀漀渀猀琀 挀栀愀爀⨀ 琀礀瀀攀一愀洀攀 㴀 瘀愀氀甀攀吀礀瀀攀ⴀ㸀䜀攀琀一愀洀攀⠀⤀㬀 ††††††††† 

            std::streamsize w = 40਻††††††††††਀            椀昀 ⠀　 ℀㴀 漀甀琀⸀眀椀搀琀栀⠀⤀⤀਀            ਀††††††††††੻††††††††††††਀                眀 㴀 猀琀搀㨀㨀洀椀渀⠀漀甀琀⸀眀椀搀琀栀⠀⤀Ⰰ 眀⤀㬀 ††††††††††† 
             †††††††††素 ††††††††† 

            line << "("਻††††††††††਀਀            椀昀 ⠀਀††††††††††††††††: †††††††††††††††㨠猀琀爀氀攀渀⠀琀礀瀀攀一愀洀攀⤀ 㸀 猀琀愀琀椀挀开挀愀猀琀㰀猀椀稀攀开琀㸀⠀眀⤀⤀਀            ਀††††††††††੻††††††††††††਀                氀椀渀攀⸀眀爀椀琀攀⠀琀礀瀀攀一愀洀攀Ⰰ 眀⤀㬀 ††††††††††† 
             †††††††††素 ††††††††† 
            else
             †††††††††笠 ††††††††††† 
                line << typeName਻††††††††††††਀            ਀††††††††††੽††††††††††਀਀            氀椀渀攀 㰀㰀 ∀⤀∀㬀 ††††††††† 

            out << line.str()਻††††††††††਀        ਀††††††††੽††††††††਀    ਀††††††੽††††††਀਀    猀琀愀琀椀挀 瘀漀椀搀 倀爀椀渀琀䠀椀攀爀愀爀栀礀⠀猀琀搀਀††††††††††††††††††††††††††††††: †††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 挀栀愀爀⨀ 猀礀洀戀漀氀猀Ⰰ 猀椀稀攀开琀 氀攀瘀攀氀Ⰰ 椀渀琀 挀漀氀圀椀搀琀栀Ⰰ 戀漀漀氀 椀猀䰀愀猀琀刀漀眀⤀਀    ਀††††††੻††††††††਀        猀琀愀琀椀挀 挀漀渀猀琀 挀栀愀爀⨀ 搀攀昀愀甀氀琀匀礀洀戀漀氀猀 㴀 ∀    ∀㬀 ††††††† 

        if (nullptr == symbols)
         †††††††笠 ††††††††† 
            symbols = defaultSymbols਻††††††††††਀        ਀††††††††੽††††††††਀਀        椀昀 ⠀氀攀瘀攀氀 㸀 　⤀਀        ਀††††††††੻††††††††††਀            昀漀爀 ⠀猀椀稀攀开琀 椀 㴀 　㬀 椀 㰀 氀攀瘀攀氀 ⴀ ㄀㬀 ⬀⬀椀⤀਀            ਀††††††††††੻††††††††††††਀                漀甀琀 㰀㰀 猀琀搀㨀㨀猀攀琀眀⠀挀漀氀圀椀搀琀栀⤀㬀 ††††††††††† 
                out << std::left << symbols[1]਻††††††††††††਀            ਀††††††††††੽††††††††††਀਀            椀昀 ⠀℀椀猀䰀愀猀琀刀漀眀⤀਀            ਀††††††††††੻††††††††††††਀                漀甀琀 㰀㰀 猀琀搀㨀㨀猀攀琀眀⠀挀漀氀圀椀搀琀栀⤀㬀 ††††††††††† 
                out << std::setfill(symbols[0])਻††††††††††††਀                漀甀琀 㰀㰀 猀琀搀㨀㨀氀攀昀琀 㰀㰀 猀礀洀戀漀氀猀嬀㈀崀㬀 ††††††††††† 
             †††††††††素 ††††††††† 
            else
             †††††††††笠 ††††††††††† 
                out << std::setw(colWidth)਻††††††††††††਀                漀甀琀 㰀㰀 猀琀搀㨀㨀猀攀琀昀椀氀氀⠀猀礀洀戀漀氀猀嬀　崀⤀㬀 ††††††††††† 
                out << std::left << symbols[3]਻††††††††††††਀            ਀††††††††††੽††††††††††਀        ਀††††††††੽††††††††਀਀        漀甀琀 㰀㰀 猀琀搀㨀㨀猀攀琀昀椀氀氀⠀✀ ✀⤀㬀 ††††††† 
     †††††素 ††††† 

    static void Dump(std ††††††††††††††††††††㨠਀†††††††††††††††††††††:ostream& out, const Reflection ††††††††††††††††††††㨠਀†††††††††††††††††††††:Field& field, size_t level, size_t maxlevel, bool isLastRow = false)
     †††††笠 ††††††† 
        if (level <= maxlevel || 0 == maxlevel)
         †††††††笠 ††††††††† 
            const size_t hierarchyColWidth = 4਻††††††††††਀            挀漀渀猀琀 猀椀稀攀开琀 渀愀洀攀䌀漀氀圀椀搀琀栀 㴀 ㌀　㬀 ††††††††† 
            const size_t valueColWidth = 25਻††††††††††਀            挀漀渀猀琀 猀椀稀攀开琀 琀礀瀀攀䌀漀氀圀椀搀琀栀 㴀 ㈀　㬀 ††††††††† 

            std::ostringstream line਻††††††††††਀਀            戀漀漀氀 栀愀猀䌀栀椀氀搀爀攀渀 㴀 昀椀攀氀搀⸀爀攀昀⸀䤀猀嘀愀氀椀搀⠀⤀ ☀☀ 昀椀攀氀搀⸀爀攀昀⸀䠀愀猀䘀椀攀氀搀猀⠀⤀㬀 ††††††††† 

            // print hierarchy
            PrintHierarhy(line, nullptr, level, hierarchyColWidth, isLastRow)਻††††††††††਀਀            ⼀⼀ 瀀爀椀渀琀 欀攀礀਀            氀椀渀攀 㰀㰀 猀琀搀㨀㨀猀攀琀眀⠀渀愀洀攀䌀漀氀圀椀搀琀栀 ⴀ 氀攀瘀攀氀 ⨀ 栀椀攀爀愀爀挀栀礀䌀漀氀圀椀搀琀栀⤀ 㰀㰀 猀琀搀㨀㨀氀攀昀琀㬀 ††††††††† 
            if (0 == maxlevel || !hasChildren)
             †††††††††笠 ††††††††††† 
                DumpAny(line, field.key)਻††††††††††††਀            ਀††††††††††੽††††††††††਀            攀氀猀攀਀            ਀††††††††††੻††††††††††††਀                猀琀搀㨀㨀漀猀琀爀椀渀最猀琀爀攀愀洀 渀愀洀攀㬀 ††††††††††† 
                DumpAny(name, field.key)਻††††††††††††਀਀                椀昀 ⠀⠀氀攀瘀攀氀 ⬀ ㄀⤀ 㰀㴀 洀愀砀氀攀瘀攀氀⤀਀                ਀††††††††††††੻††††††††††††††਀                    氀椀渀攀 㰀㰀 渀愀洀攀⸀猀琀爀⠀⤀ ⬀ ∀嬀ⴀ崀∀㬀 ††††††††††††† 
                 †††††††††††素 ††††††††††† 
                else
                 †††††††††††笠 ††††††††††††† 
                    line << name.str() + "[+]"਻††††††††††††††਀                ਀††††††††††††੽††††††††††††਀            ਀††††††††††੽††††††††††਀਀            ⼀⼀ 搀攀氀椀洀椀琀攀爀਀            氀椀渀攀 㰀㰀 ∀ 㴀 ∀㬀 ††††††††† 

            // print value
            line << std::setw(valueColWidth) << std::left਻††††††††††਀            䐀甀洀瀀刀攀昀⠀氀椀渀攀Ⰰ 昀椀攀氀搀⸀爀攀昀⤀㬀 ††††††††† 

            // print type
            line << std::setw(typeColWidth)਻††††††††††਀            䐀甀洀瀀吀礀瀀攀⠀氀椀渀攀Ⰰ 昀椀攀氀搀⸀爀攀昀⤀㬀 ††††††††† 

            // endl
            out << line.str() << std::endl਻††††††††††਀਀            ⼀⼀ 挀栀椀氀搀爀攀渀਀            椀昀 ⠀栀愀猀䌀栀椀氀搀爀攀渀⤀਀            ਀††††††††††੻††††††††††††਀                嘀攀挀琀漀爀㰀刀攀昀氀攀挀琀椀漀渀㨀㨀䘀椀攀氀搀㸀 挀栀椀氀搀爀攀渀 㴀 昀椀攀氀搀⸀爀攀昀⸀䜀攀琀䘀椀攀氀搀猀⠀⤀㬀 ††††††††††† 
                for (size_t i = 0; i < children.size(); ++i)
                 †††††††††††笠 ††††††††††††† 
                    bool isLast = (i == (children.size() - 1))਻††††††††††††††਀                    䐀甀洀瀀⠀漀甀琀Ⰰ 挀栀椀氀搀爀攀渀嬀椀崀Ⰰ 氀攀瘀攀氀 ⬀ ㄀Ⰰ 洀愀砀氀攀瘀攀氀Ⰰ 椀猀䰀愀猀琀⤀㬀 ††††††††††††† 
                 †††††††††††素 ††††††††††† 
             †††††††††素 ††††††††† 
         †††††††素 ††††††† 
     †††††素 ††††† 
 †††素 ††† ਻††††਀਀挀漀渀猀琀 䐀甀洀瀀攀爀㨀㨀倀爀椀渀琀攀爀猀吀愀戀氀攀 䐀甀洀瀀攀爀㨀㨀瘀愀氀甀攀倀爀椀渀琀攀爀猀 㴀 ਀††††੻††††††਀    ਀††††††੻†††††††† 吀礀瀀攀㨀㨀䤀渀猀琀愀渀挀攀㰀椀渀琀㌀㈀㸀⠀⤀Ⰰ 嬀崀⠀猀琀搀਀†††††††††††††††††††††††††††††††††††††: ††††††††††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀ 愀渀礀⤀ ਀††††††††੻†††††††††† 漀甀琀 㰀㰀 愀渀礀⸀䜀攀琀㰀椀渀琀㌀㈀㸀⠀⤀㬀 †††††††††   †††††††素 †††††††   †††††素 ††††† ,
     †††††笠 †††††††  Type::Instance<uint32>(), [](std †††††††††††††††††††††††††††††††††††††㨠਀††††††††††††††††††††††††††††††††††††††:ostringstream& out, const Any& any)  †††††††笠 †††††††††  out << any.Get<uint32>()਻†††††††††† ਀††††††††੽†††††††† ਀††††††੽††††††Ⰰ਀    ਀††††††੻†††††††† 吀礀瀀攀㨀㨀䤀渀猀琀愀渀挀攀㰀椀渀琀㘀㐀㸀⠀⤀Ⰰ 嬀崀⠀猀琀搀਀†††††††††††††††††††††††††††††††††††††: ††††††††††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀ 愀渀礀⤀ ਀††††††††੻†††††††††† 漀甀琀 㰀㰀 愀渀礀⸀䜀攀琀㰀椀渀琀㘀㐀㸀⠀⤀㬀 †††††††††   †††††††素 †††††††   †††††素 ††††† ,
     †††††笠 †††††††  Type::Instance<uint64>(), [](std †††††††††††††††††††††††††††††††††††††㨠਀††††††††††††††††††††††††††††††††††††††:ostringstream& out, const Any& any)  †††††††笠 †††††††††  out << any.Get<uint64>()਻†††††††††† ਀††††††††੽†††††††† ਀††††††੽††††††Ⰰ਀    ਀††††††੻†††††††† 吀礀瀀攀㨀㨀䤀渀猀琀愀渀挀攀㰀昀氀漀愀琀㌀㈀㸀⠀⤀Ⰰ 嬀崀⠀猀琀搀਀†††††††††††††††††††††††††††††††††††††††: ††††††††††††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀ 愀渀礀⤀ ਀††††††††੻†††††††††† 漀甀琀 㰀㰀 愀渀礀⸀䜀攀琀㰀昀氀漀愀琀㌀㈀㸀⠀⤀㬀 †††††††††   †††††††素 †††††††   †††††素 ††††† ,
     †††††笠 †††††††  Type::Instance<float64>(), [](std ††††††††††††††††††††††††††††††††††††††㨠਀†††††††††††††††††††††††††††††††††††††††:ostringstream& out, const Any& any)  †††††††笠 †††††††††  out << any.Get<float64>()਻†††††††††† ਀††††††††੽†††††††† ਀††††††੽††††††Ⰰ਀    ਀††††††੻†††††††† 吀礀瀀攀㨀㨀䤀渀猀琀愀渀挀攀㰀匀琀爀椀渀最㸀⠀⤀Ⰰ 嬀崀⠀猀琀搀਀††††††††††††††††††††††††††††††††††††††: †††††††††††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀ 愀渀礀⤀ ਀††††††††੻†††††††††† 漀甀琀 㰀㰀 愀渀礀⸀䜀攀琀㰀匀琀爀椀渀最㸀⠀⤀⸀挀开猀琀爀⠀⤀㬀 †††††††††   †††††††素 †††††††   †††††素 ††††† ,
     †††††笠 †††††††  Type::Instance<size_t>(), [](std †††††††††††††††††††††††††††††††††††††㨠਀††††††††††††††††††††††††††††††††††††††:ostringstream& out, const Any& any)  †††††††笠 †††††††††  out << any.Get<size_t>()਻†††††††††† ਀††††††††੽†††††††† ਀††††††੽††††††Ⰰ਀    ਀††††††੻†††††††† 吀礀瀀攀㨀㨀䤀渀猀琀愀渀挀攀㰀瘀漀椀搀㸀⠀⤀Ⰰ 嬀崀⠀猀琀搀਀††††††††††††††††††††††††††††††††††††: †††††††††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀ 愀渀礀⤀ ਀††††††††੻†††††††††† 漀甀琀 㰀㰀 ∀㼀㼀㼀∀㬀 †††††††††   †††††††素 †††††††   †††††素 ††††† 
 †††素 ††† ਻††††਀਀挀漀渀猀琀 䐀甀洀瀀攀爀㨀㨀倀爀椀渀琀攀爀猀吀愀戀氀攀 䐀甀洀瀀攀爀㨀㨀瀀漀椀渀琀攀爀倀爀椀渀琀攀爀猀 㴀 ਀††††੻††††††਀    ਀††††††੻†††††††† 吀礀瀀攀㨀㨀䤀渀猀琀愀渀挀攀㰀挀栀愀爀㸀⠀⤀Ⰰ 嬀崀⠀猀琀搀਀††††††††††††††††††††††††††††††††††††: †††††††††††††††††††††††††††††††††††㨠漀猀琀爀椀渀最猀琀爀攀愀洀☀ 漀甀琀Ⰰ 挀漀渀猀琀 䄀渀礀☀ 愀渀礀⤀ ਀††††††††੻†††††††††† 漀甀琀 㰀㰀 愀渀礀⸀䜀攀琀㰀挀漀渀猀琀 挀栀愀爀⨀㸀⠀⤀㬀 †††††††††   †††††††素 †††††††   †††††素 ††††† ,
     †††††笠 †††††††  Type::Instance<void>(), [](std †††††††††††††††††††††††††††††††††††㨠਀††††††††††††††††††††††††††††††††††††:ostringstream& out, const Any& any)  †††††††笠 †††††††††  out << "0x" << std::setw(8) << std::setfill('0') << std::hex << any.Get<void*>()਻†††††††††† ਀††††††††੽†††††††† ਀††††††੽††††††਀਀††††੽††††㬀 ††† 

 †素 †  // ReflectionDetail

void Reflection::Dump(std †††††††††††††††††††††㨠਀††††††††††††††††††††††:ostream& out, size_t maxlevel) const
 †笠 ††† 
    ReflectionDetail::Dumper::Dump(out, ⁻ ∀琀栀椀猀∀Ⰰ ⨀琀栀椀猀  }, 0, maxlevel)਻††††਀਀††੽††਀਀瘀漀椀搀 刀攀昀氀攀挀琀椀漀渀㨀㨀䐀甀洀瀀䴀攀琀栀漀搀猀⠀猀琀搀਀†††††††††††††††††††††††††††††: ††††††††††††††††††††††††††††㨠漀猀琀爀攀愀洀☀ 漀甀琀⤀ 挀漀渀猀琀਀਀††੻††††਀    嘀攀挀琀漀爀㰀䴀攀琀栀漀搀㸀 洀攀琀栀漀搀猀 㴀 䜀攀琀䴀攀琀栀漀搀猀⠀⤀㬀 ††† 
    for (auto& method  ††††††††㨠 洀攀琀栀漀搀猀⤀਀    ਀††††੻††††††਀        挀漀渀猀琀 䄀渀礀䘀渀㨀㨀䤀渀瘀漀欀攀倀愀爀愀洀猀☀ 瀀愀爀愀洀猀 㴀 洀攀琀栀漀搀⸀昀渀⸀䜀攀琀䤀渀瘀漀欀攀倀愀爀愀洀猀⠀⤀㬀 ††††† 

        out << params.retType->GetName() << " "਻††††††਀        漀甀琀 㰀㰀 洀攀琀栀漀搀⸀欀攀礀 㰀㰀 ∀⠀∀㬀 ††††† 

        for (size_t i = 0; i < params.argsType.size(); ++i)
         †††††笠 ††††††† 
            out << params.argsType[i]->GetName()਻††††††††਀਀            椀昀 ⠀椀 㰀 ⠀瀀愀爀愀洀猀⸀愀爀最猀吀礀瀀攀⸀猀椀稀攀⠀⤀ ⴀ ㄀⤀⤀਀            ਀††††††††੻††††††††††਀                漀甀琀 㰀㰀 ∀Ⰰ ∀㬀 ††††††††† 
             †††††††素 ††††††† 
         †††††素 ††††† 

        out << ");" << std::endl਻††††††਀    ਀††††੽††††਀਀††੽††਀਀਀੽ ⼀⼀ 渀愀洀攀猀瀀愀挀攀 䐀䄀嘀䄀਀�