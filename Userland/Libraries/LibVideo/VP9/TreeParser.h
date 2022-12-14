/*
 * Copyright (c) 2021, Hunter Salyer <thefalsehonesty@gmail.com>
 * Copyright (c) 2022, Gregory Bertilson <zaggy1024@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "BitStream.h"
#include "Enums.h"
#include "ProbabilityTables.h"
#include "SyntaxElementCounter.h"

namespace Video::VP9 {

class Parser;

class TreeParser {
public:
    explicit TreeParser(Parser& decoder)
        : m_decoder(decoder)
    {
    }

    class TreeSelection {
    public:
        union TreeSelectionValue {
            int const* m_tree;
            int m_value;
        };

        constexpr TreeSelection(int const* values)
            : m_is_single_value(false)
            , m_value { .m_tree = values }
        {
        }

        constexpr TreeSelection(int value)
            : m_is_single_value(true)
            , m_value { .m_value = value }
        {
        }

        bool is_single_value() const { return m_is_single_value; }
        int get_single_value() const { return m_value.m_value; }
        int const* get_tree_value() const { return m_value.m_tree; }

    private:
        bool m_is_single_value;
        TreeSelectionValue m_value;
    };

    /* (9.3.3) */
    template<typename T = int>
    ErrorOr<T> parse_tree(SyntaxElementType type);
    /* (9.3.1) */
    TreeSelection select_tree(SyntaxElementType type);
    /* (9.3.2) */
    u8 select_tree_probability(SyntaxElementType type, u8 node);
    /* (9.3.4) */
    void count_syntax_element(SyntaxElementType type, int value);

    void set_default_intra_mode_variables(u8 idx, u8 idy)
    {
        m_idx = idx;
        m_idy = idy;
    }

    void set_tokens_variables(u8 band, u32 c, u32 plane, TXSize tx_size, u32 pos);

    void set_start_x_and_y(u32 start_x, u32 start_y)
    {
        m_start_x = start_x;
        m_start_y = start_y;
    }

    void set_mv_component(u8 component)
    {
        m_mv_component = component;
    }

    ErrorOr<bool> parse_mv_bit(u8 bit)
    {
        m_mv_bit = bit;
        return parse_tree<bool>(SyntaxElementType::MVBit);
    }

    ErrorOr<u8> parse_mv_class0_fr(bool mv_class0_bit)
    {
        m_mv_class0_bit = mv_class0_bit;
        return parse_tree<u8>(SyntaxElementType::MVClass0FR);
    }

private:
    u8 calculate_partition_probability(u8 node);
    u8 calculate_default_intra_mode_probability(u8 node);
    u8 calculate_default_uv_mode_probability(u8 node);
    u8 calculate_intra_mode_probability(u8 node);
    u8 calculate_sub_intra_mode_probability(u8 node);
    u8 calculate_uv_mode_probability(u8 node);
    u8 calculate_segment_id_probability(u8 node);
    u8 calculate_skip_probability();
    u8 calculate_seg_id_predicted_probability();
    u8 calculate_is_inter_probability();
    u8 calculate_comp_mode_probability();
    u8 calculate_comp_ref_probability();
    u8 calculate_single_ref_p1_probability();
    u8 calculate_single_ref_p2_probability();
    u8 calculate_tx_size_probability(u8 node);
    u8 calculate_inter_mode_probability(u8 node);
    u8 calculate_interp_filter_probability(u8 node);
    u8 calculate_token_probability(u8 node);
    u8 calculate_more_coefs_probability();

    Parser& m_decoder;
    // m_ctx is a member variable because it is required for syntax element counting (section 9.3.4)
    u8 m_ctx { 0 };

    // These are variables necessary for parsing tree data, but aren't useful otherwise, so they're
    // not stored in the Decoder itself.
    u8 m_idx { 0 };
    u8 m_idy { 0 };
    u8 m_band { 0 };
    u32 m_start_x { 0 };
    u32 m_start_y { 0 };
    u32 m_c { 0 };
    u32 m_plane { 0 };
    TXSize m_tx_size;
    u32 m_pos { 0 };
    u8 m_mv_component { 0 };
    // 0xFF indicates the value has not been set.
    // parse_mv_bit should be called to set this.
    u8 m_mv_bit { 0xFF };
    // 0xFF indicates the value has not been set.
    // parse_mv_class0_fr should be called to set this.
    u8 m_mv_class0_bit { 0xFF };
};

}
