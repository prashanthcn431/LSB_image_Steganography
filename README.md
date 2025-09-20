# LSB_image_Steganography


<h2>🚀 How to Run</h2>

<b>1️⃣ Compile the Project</b>
<pre><code>make</code></pre>

<b>2️⃣ Encoding a Message</b>
<pre><code>./steg encode &lt;source.bmp&gt; &lt;secret.txt&gt; &lt;output.bmp&gt;</code></pre>
Example:
<pre><code>./steg encode input.bmp secret.txt encoded.bmp</code></pre>

<b>3️⃣ Decoding a Message</b>
<pre><code>./steg decode &lt;encoded.bmp&gt; &lt;output.txt&gt;</code></pre>
Example:
<pre><code>./steg decode encoded.bmp decoded.txt</code></pre>

<h2>📝 Example Output</h2>

<b>Encoding:</b>
<pre>
INFO: Starting Encoding Process...  
INFO: Copying BMP Header...  
INFO: Embedding Secret Message Length...  
INFO: Encoding Secret Message...  
INFO: Saving Encoded Image...  
INFO: Encoding Completed Successfully!  
</pre>

<b>Decoding:</b>
<pre>
INFO: Starting Decoding Process...  
INFO: Reading BMP Header...  
INFO: Extracting Secret Message Length...  
INFO: Decoding Secret Message...  
INFO: Decoding Completed Successfully!  
</pre>

<h2>🔍 How It Works – LSB Technique</h2>
<p>
The <b>Least Significant Bit</b> of each byte in the image is modified to store bits of the secret message.  
Since the LSB change is visually negligible, the message is hidden without altering image quality.  
</p>

<h3>Encoding Flow</h3>
<ol>
  <li>Read BMP header and copy it to the output file.</li>
  <li>Embed the secret message length into pixel data.</li>
  <li>Embed each bit of the secret message into LSBs of the image.</li>
  <li>Save the new BMP file.</li>
</ol>

<h3>Decoding Flow</h3>
<ol>
  <li>Read BMP header.</li>
  <li>Extract message length from LSBs.</li>
  <li>Read bits from LSBs and reconstruct the secret message.</li>
  <li>Save decoded message to a text file.</li>
</ol>

<hr>

<p align="center">✨ Built with ❤️ using C and the magic of Steganography ✨</p>
