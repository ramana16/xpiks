/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FLAGS
#define FLAGS

#include <type_traits>
#include <QObject>

namespace Common {
    typedef uint32_t flag_t;

    template<typename FlagType>
    struct enable_bitmask_operators {
        static constexpr bool enable=false;
    };

    template<typename FlagType>
    typename std::enable_if<enable_bitmask_operators<FlagType>::enable, FlagType>::type
    operator|(FlagType a, FlagType b) {
        typedef typename std::underlying_type<FlagType>::type underlying;
        return static_cast<FlagType>(static_cast<underlying>(a) | static_cast<underlying>(b));
    }

    template<typename FlagType>
    typename std::enable_if<enable_bitmask_operators<FlagType>::enable, FlagType>::type
    &operator|=(FlagType &a, FlagType b) {
        typedef typename std::underlying_type<FlagType>::type underlying;
        a = static_cast<FlagType>(static_cast<underlying>(a) | static_cast<underlying>(b));
        return a;
    }

    enum struct CombinedEditFlags: flag_t {
        None = 0,
        EditTitle = 1 << 0,
        EditDescription = 1 << 1,
        EditKeywords = 1 << 2,
        AppendKeywords = 1 << 3,
        Clear = 1 << 4,
        EditEverything = EditTitle | EditDescription | EditKeywords
    };

    enum struct SuggestionFlags: flag_t {
        None = 0,
        Title = 1 << 0,
        Description = 1 << 1,
        Keywords = 1 << 2,
        All = Title | Description | Keywords
    };

    enum struct SpellCheckFlags: flag_t {
        Title = 1 << 0,
        Description = 1 << 1,
        Keywords = 1 << 2,
        All = Title | Description | Keywords
    };

    enum struct KeywordReplaceResult: flag_t {
        Succeeded = 0,
        FailedIndex = 1,
        FailedDuplicate = 2,
        Unknown = 1 << 20
    };

    enum struct SearchFlags: flag_t {
        None = 0,
        CaseSensitive = 1 << 0,
        Description = 1 << 1,
        Title = 1 << 2,
        Keywords = 1 << 3,
        ReservedTerms = 1 << 4, // include reserved terms like "x:empty"
        AllTerms = 1 << 5, // all of the search terms included in the result
        Filepath = 1 << 6,
        WholeWords = 1 << 7,
        IncludeSpaces = 1 << 8, // includes spaces inside

        Metadata = Description | Title | Keywords,
        ExactKeywords = WholeWords | Keywords,
        MetadataCaseSensitive = Metadata | CaseSensitive,

        Everything = Metadata | Filepath | ReservedTerms,
        AllTermsEverything = Everything | AllTerms,
        AnyTermsEverything = Everything
    };

#ifdef CORE_TESTS
    template<>
    struct enable_bitmask_operators<SearchFlags> {
        static constexpr bool enable = true;
    };

    template<>
    struct enable_bitmask_operators<CombinedEditFlags> {
        static constexpr bool enable = true;
    };
#endif

    enum struct WarningFlags: flag_t {
        None = 0,
        SizeLessThanMinimum = 1 << 0,
        NoKeywords = 1 << 1,
        TooFewKeywords = 1 << 2,
        TooManyKeywords = 1 << 3,
        DescriptionIsEmpty = 1 << 4,
        DescriptionNotEnoughWords = 1 << 5,
        DescriptionTooBig = 1 << 6,
        TitleIsEmpty = 1 << 7,
        TitleNotEnoughWords = 1 << 8,
        TitleTooManyWords = 1 << 9,
        TitleTooBig = 1 << 10,
        SpellErrorsInKeywords = 1 << 11,
        SpellErrorsInDescription = 1 << 12,
        SpellErrorsInTitle = 1 << 13,
        FileIsTooBig = 1 << 14,
        KeywordsInDescription = 1 << 15,
        KeywordsInTitle = 1 << 16,
        FilenameSymbols = 1 << 17,

        DescriptionGroup = DescriptionIsEmpty |
            DescriptionNotEnoughWords |
            DescriptionTooBig |
            SpellErrorsInDescription |
            KeywordsInDescription,

        TitleGroup = TitleIsEmpty |
            TitleNotEnoughWords |
            TitleTooManyWords |
            TitleTooBig |
            SpellErrorsInTitle |
            KeywordsInTitle,

        KeywordsGroup = NoKeywords |
            TooFewKeywords |
            TooManyKeywords |
            SpellErrorsInKeywords |
            KeywordsInDescription |
            KeywordsInTitle,

        SpellingGroup = SpellErrorsInKeywords |
            SpellErrorsInDescription |
            SpellErrorsInTitle
    };

    template<>
    struct enable_bitmask_operators<WarningFlags> {
        static constexpr bool enable = true;
    };

    enum struct WarningsCheckFlags {
        All = 0,
        Keywords = 1,
        Title = 2,
        Description = 3,
        Spelling = 4
    };

    enum struct WordAnalysisFlags: flag_t {
        None = 0,
        Spelling = 1 << 0,
        Stemming = 1 << 1,
        All = Spelling | Stemming
    };

    const char *warningsFlagToString(WarningsCheckFlags flags);

    template<typename FlagType>
    bool HasFlag(flag_t value, FlagType flag) {
        flag_t intFlag = static_cast<flag_t>(flag);
        bool result = (value & intFlag) == intFlag;
        return result;
    }

    template<typename FlagType>
    bool HasFlag(FlagType value, FlagType flag) {
        flag_t intValue = static_cast<flag_t>(value);
        flag_t intFlag = static_cast<flag_t>(flag);
        bool result = (intValue & intFlag) == intFlag;
        return result;
    }

    template<typename FlagType>
    void SetFlag(flag_t &value, FlagType flag) {
        value |= static_cast<flag_t>(flag);
    }

    template<typename FlagType>
    void SetFlag(FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<flag_t>(value) | static_cast<flag_t>(flag));
    }

    template<typename FlagType>
    void SetFlag(volatile flag_t &value, FlagType flag) {
        value |= static_cast<flag_t>(flag);
    }

    template<typename FlagType>
    void SetFlag(volatile FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<flag_t>(value) | static_cast<flag_t>(flag));
    }

    template<typename FlagType>
    void UnsetFlag(flag_t &value, FlagType flag) {
        value &= ~(static_cast<flag_t>(flag));
    }

    template<typename FlagType>
    void UnsetFlag(FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<flag_t>(value) & (~(static_cast<flag_t>(flag))));
    }

    template<typename FlagType>
    void UnsetFlag(volatile flag_t &value, FlagType flag) {
        value &= ~(static_cast<flag_t>(flag));
    }

    template<typename FlagType>
    void UnsetFlag(volatile FlagType &value, FlagType flag) {
        value = static_cast<FlagType>(static_cast<flag_t>(value) & (~(static_cast<flag_t>(flag))));
    }

    template<typename FlagType>
    void ApplyFlag(flag_t &value, bool applySwitch, FlagType flag) {
        if (applySwitch) {
            SetFlag(value, flag);
        } else {
            UnsetFlag(value, flag);
        }
    }

    template<typename FlagType>
    void ApplyFlag(FlagType &value, bool applySwitch, FlagType flag) {
        if (applySwitch) {
            SetFlag(value, flag);
        } else {
            UnsetFlag(value, flag);
        }
    }

    template<typename FlagType>
    void ApplyFlag(volatile flag_t &value, bool applySwitch, FlagType flag) {
        if (applySwitch) {
            SetFlag(value, flag);
        } else {
            UnsetFlag(value, flag);
        }
    }
}

Q_DECLARE_METATYPE(Common::SpellCheckFlags)

#endif // FLAGS

