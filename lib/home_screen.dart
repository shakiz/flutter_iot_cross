import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'package:pdf/pdf.dart';
import 'package:pdf/widgets.dart' as pw;
import 'package:printing/printing.dart';

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  MethodChannel methodChannel = const MethodChannel('com.example.dll_executor');

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(widget.title),
      ),
      body: PdfPreview(
        build: (format) => _generatePdf(format, "Print Doc"),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          // Open a dll file from here
          executeDllFunction(context);
        },
        tooltip: 'Open DLL File',
        child: const Icon(Icons.window),
      ),
    );
  }

  Future<void> executeDllFunction(BuildContext context) async {
    try {
      final result = await methodChannel.invokeMethod('executeDllFunction', {
        'functionName': 'YourFunctionName',
      });
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(
        content: Text(result),
      ));
      debugPrint(result);
    } catch (e) {
      debugPrint("Error: $e");
    }
  }

  Future<Uint8List> _generatePdf(PdfPageFormat format, String title) async {
    final pdf = pw.Document(version: PdfVersion.pdf_1_5, compress: true);
    final font = await PdfGoogleFonts.nunitoExtraLight();

    pdf.addPage(
      pw.Page(
        pageFormat: format,
        build: (context) {
          return pw.Column(
            children: [
              pw.SizedBox(
                width: double.infinity,
                child: pw.FittedBox(
                  child: pw.Text(title, style: pw.TextStyle(font: font)),
                ),
              ),
              pw.SizedBox(height: 20),
              pw.Flexible(child: pw.FlutterLogo()),
            ],
          );
        },
      ),
    );

    return pdf.save();
  }
}
