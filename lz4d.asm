lz4_decode:
    addq.l  #7,a0
    lea     endian_data+4(pc),a2

    moveq.l #0,d2
data_block_loop:
    move.b  (a0)+,-(a2)
    move.b  (a0)+,-(a2)
    move.b  (a0)+,-(a2)
    move.b  (a0)+,-(a2)
    move.l  (a2)+,d3
    bmi.s   block_copy
    beq.s   end_of_stream

    lea     (a0,d3.l),a4

lz4_block_loop:
    moveq.l #0,d0
    move.b  (a0)+,d0

    move.w  d0,d1
    lsr.w   #4,d1
    beq.s   no_literals

    subq.w  #1,d1
    cmp.w   #15-1,d1
    bne.s   literals_copy_loop

additional_literals_length:
    move.b  (a0)+,d2
    add.w   d2,d1
    not.b   d2
    beq.s   additional_literals_length

literals_copy_loop:
    move.b  (a0)+,(a1)+

    dbf     d1,literals_copy_loop

no_literals:
    cmp.l   a4,a0
    beq.s   data_block_loop

    moveq.l #0,d3
    move.b  (a0)+,-(a2)
    move.b  (a0)+,-(a2)
    move.w  (a2)+,d3
    neg.l   d3
    lea     (a1,d3.l),a3

    and.w   #$f,d0
    addq.w  #4-1,d0
    cmp.w   #15+4-1,d0
    bne.s   copy_match_loop

additional_match_length:
    move.b  (a0)+,d2
    add.w   d2,d0
    not.b   d2
    beq.s   additional_match_length

copy_match_loop:
    move.b  (a3)+,(a1)+

    dbf     d0,copy_match_loop

    bra.s   lz4_block_loop

block_copy:
    and.l   #$7fffffff,d3

block_copy_loop:
    move.b  (a0)+,(a1)+

    subq.l  #1,d3
    bne.s   block_copy_loop

    bra.s   data_block_loop

end_of_stream:
    rts

endian_data:
    ds.l    1
