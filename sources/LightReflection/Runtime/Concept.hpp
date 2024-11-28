#pragma once

namespace Light
{
    template <class TTransferrer>
    concept Transferrer = requires(TTransferrer& transferrer, void* value) { transferrer.TransferField("value", value); };
}
