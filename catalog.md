# Catalog file format

## catalog.header

`catalog.header` contains 8 bytes.

| offset | size | meaning |
|---|---|---|
|0|4|the offset of first occurred valid block<br>if there's no valid block, then it will be `ffffffff`|
|4|4|the offset of first occurred invalid block<br>if there's no invalid block, then it will be the end of this file|

## catalog

`catalog` only contains the blocks.

For each valid block:

|offset|size|meaning|
|-|-|-|
|0|4|the next valid block's offset|
|4|4|the number of tables in this block|
|||then there will be several tables|

For each invalid block:

|offset|size|meaning|
|-|-|-|
|0|4|the next invalid block's offset|
|||the rest of the block is no need to care|