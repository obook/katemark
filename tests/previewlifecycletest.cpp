// Kate destroys a document when its editor tab closes, and it does so in two shapes:
// closing one tab runs closeUrl() first (which empties the url and then the buffer),
// while closing several at once deletes the document with its text still intact.
// Either way the preview widget outlives the document, so it has to keep rendering
// from its own copy of the source.

#include "testhelpers.h"

class PreviewLifecycleTest : public PreviewTest
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        createFixture();
    }
    void survivesDocumentClose();
    void survivesDeletionWithoutCloseUrl();
    void reattachesToReopenedDocument();
    void loadsImageBesideTheDocument();
};

// One editor tab closing: Kate calls closeUrl(), which clears the url before anything
// announces the close and empties the buffer straight after, then deletes the document.
void PreviewLifecycleTest::survivesDocumentClose()
{
    KTextEditor::Document *doc = openDocument();
    QTRY_VERIFY(doc->text().contains(Body));

    auto preview = std::make_unique<PreviewWidget>(nullptr, nullptr, doc);
    QVERIFY(waitForPageText(preview.get(), Body));

    QVERIFY(doc->closeUrl());
    delete doc;

    // Well past the render debounce and any reload the close could have kicked off.
    QTest::qWait(2000);
    const QString text = pageText(preview.get());
    QVERIFY2(text.contains(Body), qPrintable(QStringLiteral("preview went blank, article reads: '%1'").arg(text)));
}

// Closing several tabs at once takes the other path: no closeUrl, so the document is
// deleted with its url and text still intact and nothing announces it.
void PreviewLifecycleTest::survivesDeletionWithoutCloseUrl()
{
    KTextEditor::Document *doc = openDocument();
    QTRY_VERIFY(doc->text().contains(Body));

    auto preview = std::make_unique<PreviewWidget>(nullptr, nullptr, doc);
    QVERIFY(waitForPageText(preview.get(), Body));

    delete doc;

    QTest::qWait(2000);
    const QString text = pageText(preview.get());
    QVERIFY2(text.contains(Body), qPrintable(QStringLiteral("preview went blank, article reads: '%1'").arg(text)));
}

// Reopening the file hands the preview a fresh document and it goes live again.
void PreviewLifecycleTest::reattachesToReopenedDocument()
{
    KTextEditor::Document *doc = openDocument();
    QTRY_VERIFY(doc->text().contains(Body));

    auto preview = std::make_unique<PreviewWidget>(nullptr, nullptr, doc);
    QVERIFY(waitForPageText(preview.get(), Body));
    const QUrl url = preview->documentUrl();
    QCOMPARE(url, QUrl::fromLocalFile(m_path));

    QVERIFY(doc->closeUrl());
    delete doc;

    QTest::qWait(500);
    // The url survives the document.
    QCOMPARE(preview->documentUrl(), url);

    KTextEditor::Document *reopened = openDocument();
    QTRY_VERIFY(reopened->text().contains(Body));
    preview->attachDocument(reopened, nullptr);

    reopened->setText(QStringLiteral("# live again\n\nthe reattached body line.\n"));
    QVERIFY(waitForPageText(preview.get(), QLatin1String("the reattached body line")));
    delete reopened;
}

// An image referenced by a relative path has to survive LocalFileGuard, which compares the
// request's canonical path against the document folder's canonical path. Those two strings
// are produced on different code paths, and a filesystem that matches names
// case-insensitively or hands out 8.3 short names can make them differ while naming the same
// file. The guard blocks on any mismatch, so the failure is a silently missing image rather
// than an error anyone sees.
void PreviewLifecycleTest::loadsImageBesideTheDocument()
{
    KTextEditor::Document *doc = openDocument();
    QTRY_VERIFY(doc->text().contains(Body));

    auto preview = std::make_unique<PreviewWidget>(nullptr, nullptr, doc);
    QVERIFY(waitForPageText(preview.get(), Body));

    doc->setText(QStringLiteral("# katdown\n\n![red](%1)\n").arg(ImageName));
    const int width = waitForImageWidth(preview.get());
    QVERIFY2(width == ImageWidth, qPrintable(QStringLiteral("image beside the document did not load, naturalWidth is %1").arg(width)));

    delete doc;
}

QTEST_MAIN(PreviewLifecycleTest)

#include "previewlifecycletest.moc"
