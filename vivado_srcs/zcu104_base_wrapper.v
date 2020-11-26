//Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2020.1 (lin64) Build 2902540 Wed May 27 19:54:35 MDT 2020
//Date        : Mon Nov 23 11:37:36 2020
//Host        : ubuntu-desktop running 64-bit Ubuntu 18.04.5 LTS
//Command     : generate_target zcu104_base_wrapper.bd
//Design      : zcu104_base_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module zcu104_base_wrapper
   (C0_DDR4_0_act_n,
    C0_DDR4_0_adr,
    C0_DDR4_0_ba,
    C0_DDR4_0_bg,
    C0_DDR4_0_ck_c,
    C0_DDR4_0_ck_t,
    C0_DDR4_0_cke,
    C0_DDR4_0_cs_n,
    C0_DDR4_0_dm_n,
    C0_DDR4_0_dq,
    C0_DDR4_0_dqs_c,
    C0_DDR4_0_dqs_t,
    C0_DDR4_0_odt,
    C0_DDR4_0_reset_n,
    C0_SYS_CLK_0_clk_n,
    C0_SYS_CLK_0_clk_p,
    GPIO2_0_tri_i,
    GPIO_0_tri_o);
  output C0_DDR4_0_act_n;
  output [16:0]C0_DDR4_0_adr;
  output [1:0]C0_DDR4_0_ba;
  output [1:0]C0_DDR4_0_bg;
  output C0_DDR4_0_ck_c;
  output C0_DDR4_0_ck_t;
  output C0_DDR4_0_cke;
  output C0_DDR4_0_cs_n;
  inout [7:0]C0_DDR4_0_dm_n;
  inout [63:0]C0_DDR4_0_dq;
  inout [7:0]C0_DDR4_0_dqs_c;
  inout [7:0]C0_DDR4_0_dqs_t;
  output C0_DDR4_0_odt;
  output C0_DDR4_0_reset_n;
  input C0_SYS_CLK_0_clk_n;
  input C0_SYS_CLK_0_clk_p;
  input [3:0]GPIO2_0_tri_i;
  output [3:0]GPIO_0_tri_o;

  wire C0_DDR4_0_act_n;
  wire [16:0]C0_DDR4_0_adr;
  wire [1:0]C0_DDR4_0_ba;
  wire [1:0]C0_DDR4_0_bg;
  wire C0_DDR4_0_ck_c;
  wire C0_DDR4_0_ck_t;
  wire C0_DDR4_0_cke;
  wire C0_DDR4_0_cs_n;
  wire [7:0]C0_DDR4_0_dm_n;
  wire [63:0]C0_DDR4_0_dq;
  wire [7:0]C0_DDR4_0_dqs_c;
  wire [7:0]C0_DDR4_0_dqs_t;
  wire C0_DDR4_0_odt;
  wire C0_DDR4_0_reset_n;
  wire C0_SYS_CLK_0_clk_n;
  wire C0_SYS_CLK_0_clk_p;
  wire [3:0]GPIO2_0_tri_i;
  wire [3:0]GPIO_0_tri_o;

  zcu104_base zcu104_base_i
       (.C0_DDR4_0_act_n(C0_DDR4_0_act_n),
        .C0_DDR4_0_adr(C0_DDR4_0_adr),
        .C0_DDR4_0_ba(C0_DDR4_0_ba),
        .C0_DDR4_0_bg(C0_DDR4_0_bg),
        .C0_DDR4_0_ck_c(C0_DDR4_0_ck_c),
        .C0_DDR4_0_ck_t(C0_DDR4_0_ck_t),
        .C0_DDR4_0_cke(C0_DDR4_0_cke),
        .C0_DDR4_0_cs_n(C0_DDR4_0_cs_n),
        .C0_DDR4_0_dm_n(C0_DDR4_0_dm_n),
        .C0_DDR4_0_dq(C0_DDR4_0_dq),
        .C0_DDR4_0_dqs_c(C0_DDR4_0_dqs_c),
        .C0_DDR4_0_dqs_t(C0_DDR4_0_dqs_t),
        .C0_DDR4_0_odt(C0_DDR4_0_odt),
        .C0_DDR4_0_reset_n(C0_DDR4_0_reset_n),
        .C0_SYS_CLK_0_clk_n(C0_SYS_CLK_0_clk_n),
        .C0_SYS_CLK_0_clk_p(C0_SYS_CLK_0_clk_p),
        .GPIO2_0_tri_i(GPIO2_0_tri_i),
        .GPIO_0_tri_o(GPIO_0_tri_o));
endmodule
