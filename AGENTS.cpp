bool resultLessThan(const ClassificationResult &a, const ClassificationResult &b,
                    int column, Qt::SortOrder order)
{
    bool less = false;
    switch (column) { ... }
    return order == Qt::AscendingOrder ? less : !less;
}
