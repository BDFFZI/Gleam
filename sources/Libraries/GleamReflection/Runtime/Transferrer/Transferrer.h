#pragma once

namespace Gleam
{
    template <class TTransferrer>
    concept Transferrer = requires(TTransferrer& transferrer, void* value) { transferrer.TransferField("value", value); };
}
